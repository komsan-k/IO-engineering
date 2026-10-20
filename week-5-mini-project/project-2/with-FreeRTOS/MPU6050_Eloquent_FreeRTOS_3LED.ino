/* FreeRTOS extension of MPU6050_Eloquent_3LED.ino
 * ESP32 DevKit/WROOM, EloquentTinyML 3.x, tflm_esp32
 * Required in sketch folder: MPU6050Simple.h/.cpp and trained model.h
 * NOT hardware-tested; match original library versions.
 */
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "MPU6050Simple.h"
#include "model.h"
#include <tflm_esp32.h>
#include <eloquent_tinyml.h>

constexpr int SDA_PIN=4, SCL_PIN=5;
constexpr int LED_PINS[3]={25,26,27};
constexpr uint32_t SAMPLE_MS=20, PRINT_MS=200;
constexpr uint8_t STABLE_COUNT=3;
constexpr uint32_t ARENA=24*1024;

MPU6050Simple mpu;
Eloquent::TF::Sequential<2, ARENA> tf;

struct SensorFrame { float features[6]; uint32_t stampUs; uint32_t sequence; };
struct PredictionFrame { float features[6]; uint32_t stampUs; uint32_t sequence; uint32_t inferenceUs; int prediction; };
QueueHandle_t sensorQueue=nullptr, predictionQueue=nullptr;

[[noreturn]] void fatal(const char *msg) {
  Serial.printf("FATAL: %s\n",msg);
  for (;;) {
    for (int pin:LED_PINS) digitalWrite(pin,HIGH);
    delay(200);
    for (int pin:LED_PINS) digitalWrite(pin,LOW);
    delay(200);
  }
}

void displayClass(int cls) {
  for (int pin:LED_PINS) digitalWrite(pin,LOW);
  if (cls>=0 && cls<3) digitalWrite(LED_PINS[cls],HIGH);
}

// Preserves the checks and tensor contract from the original sketch.
void checkModelContract() {
  const tflite::Model *model=tflite::GetModel(model_data);
  if (model->version()!=TFLITE_SCHEMA_VERSION) fatal("Schema mismatch");
  const auto *graphs=model->subgraphs();
  if (!graphs || graphs->size()!=1) fatal("Need one subgraph");
  const auto *g=graphs->Get(0);
  if (!g->inputs() || !g->outputs() || g->inputs()->size()!=1 || g->outputs()->size()!=1)
    fatal("Expected one input and one output tensor");
  const auto *in=g->tensors()->Get(g->inputs()->Get(0));
  const auto *out=g->tensors()->Get(g->outputs()->Get(0));
  if (in->type()!=tflite::TensorType_INT8 || out->type()!=tflite::TensorType_INT8 ||
      !in->shape() || in->shape()->size()!=2 ||
      in->shape()->Get(0)!=1 || in->shape()->Get(1)!=6 ||
      !out->shape() || out->shape()->size()!=2 ||
      out->shape()->Get(0)!=1 || out->shape()->Get(1)!=3)
    fatal("Need INT8 input [1,6] and output [1,3]");
  const auto *q=in->quantization();
  if (!q || !q->scale() || !q->zero_point() || q->scale()->size()!=1 ||
      q->zero_point()->size()!=1 || !isfinite(mpu6050_input_scale) ||
      mpu6050_input_scale<=0 ||
      fabsf(q->scale()->Get(0)-mpu6050_input_scale)>1e-7f ||
      q->zero_point()->Get(0)!=mpu6050_input_zero_point)
    fatal("Quantization parameters mismatch");
  for(int i=0;i<6;i++)
    if(!isfinite(mpu6050_mean[i]) || !isfinite(mpu6050_std[i]) || mpu6050_std[i]<=0)
      fatal("Invalid normalization parameters");
}

// Task 1: periodic MPU6050 readings (I2C is used ONLY here).
void SensorTask(void *) {
  TickType_t last=xTaskGetTickCount();
  TickType_t period=pdMS_TO_TICKS(SAMPLE_MS);
  if(period==0) { Serial.println("Tick resolution too coarse"); vTaskDelete(nullptr); }
  uint32_t seq=0;
  for(;;) {
    vTaskDelayUntil(&last,period);
    SensorFrame frame={};
    if(!mpu.readFeatures(frame.features)) {
      Serial.printf("MPU6050 read error: %s\n",mpu.errorMessage());
      continue;
    }
    frame.sequence=++seq;
    frame.stampUs=micros();
    xQueueOverwrite(sensorQueue,&frame); // latest only
  }
}

// Task 2: owns EloquentTinyML. Never invoke tf.predict in other tasks.
void InferenceTask(void *) {
  SensorFrame sensor;
  for(;;) {
    if(xQueueReceive(sensorQueue,&sensor,portMAX_DELAY)!=pdTRUE) continue;
    int8_t input[6];
    mpu6050_prepare_input(sensor.features,input); // original model.h
    const uint32_t start=micros();
    if(!tf.predict(input).isOk()) {
      Serial.println(tf.exception.toString());
      Serial.println("Inference failed; restarting");
      delay(200); ESP.restart();
    }
    PredictionFrame result={};
    result.prediction=static_cast<int>(tf.classification);
    if(result.prediction<0 || result.prediction>=3) {
      Serial.println("Invalid class; restarting"); ESP.restart();
    }
    result.inferenceUs=micros()-start;
    result.sequence=sensor.sequence;
    result.stampUs=sensor.stampUs;
    memcpy(result.features,sensor.features,sizeof(result.features));
    xQueueOverwrite(predictionQueue,&result);
  }
}

// Task 3: owns LEDs and retains original three-prediction stabilization.
void LEDTask(void *) {
  int displayed=-1, candidate=-1;
  uint8_t consecutive=0;
  uint32_t lastPrint=0;
  PredictionFrame frame;
  for(;;) {
    if(xQueueReceive(predictionQueue,&frame,portMAX_DELAY)!=pdTRUE) continue;
    if(frame.prediction!=candidate) { candidate=frame.prediction; consecutive=1; }
    else if(consecutive<STABLE_COUNT) consecutive++;
    if(consecutive>=STABLE_COUNT && displayed!=candidate) {
      displayed=candidate;
      displayClass(displayed);
    }
    const uint32_t now=millis();
    if((uint32_t)(now-lastPrint)>=PRINT_MS) {
      lastPrint=now;
      Serial.printf("seq=%lu pred=%s LED=%s inference=%lu us data_age=%lu us "
                    "acc_g=[%.3f %.3f %.3f] gyro_dps=[%.2f %.2f %.2f]\n",
                    (unsigned long)frame.sequence,
                    mpu6050_labels[frame.prediction],
                    displayed>=0?mpu6050_labels[displayed]:"waiting",
                    (unsigned long)frame.inferenceUs,
                    (unsigned long)(micros()-frame.stampUs),
                    frame.features[0],frame.features[1],frame.features[2],
                    frame.features[3],frame.features[4],frame.features[5]);
      if(frame.inferenceUs>=SAMPLE_MS*1000UL)
        Serial.println("WARNING: inference alone exceeded sampling period");
    }
  }
}

void setup() {
  Serial.begin(115200);
  for(int pin:LED_PINS) { pinMode(pin,OUTPUT); digitalWrite(pin,LOW); }
  delay(500);
  Serial.println("MPU6050 + EloquentTinyML INT8 + FreeRTOS");
  Wire.begin(SDA_PIN,SCL_PIN);
  Wire.setClock(100000);
  Wire.setTimeOut(50);
  if(!mpu.begin(Wire,0x68,MPU6050Simple::AccelRange::G4,
                MPU6050Simple::GyroRange::DPS500)) fatal(mpu.errorMessage());
  checkModelContract();
  tf.setNumInputs(6);
  tf.setNumOutputs(3);
  if(tf.resolver.AddFullyConnected()!=kTfLiteOk ||
     tf.resolver.AddSoftmax()!=kTfLiteOk) fatal("Operator registration failed");
  if(!tf.begin(model_data).isOk()) {
    Serial.println(tf.exception.toString());
    fatal("Model initialization failed");
  }
  sensorQueue=xQueueCreate(1,sizeof(SensorFrame));
  predictionQueue=xQueueCreate(1,sizeof(PredictionFrame));
  if(!sensorQueue || !predictionQueue) fatal("Queue allocation failed");
  // Classic dual-core ESP32 only. For another board revise core affinity.
  if(xTaskCreatePinnedToCore(SensorTask,"SensorTask",4096,nullptr,3,nullptr,0)!=pdPASS)
    fatal("SensorTask creation failed");
  if(xTaskCreatePinnedToCore(InferenceTask,"InferenceTask",8192,nullptr,2,nullptr,1)!=pdPASS)
    fatal("InferenceTask creation failed");
  if(xTaskCreatePinnedToCore(LEDTask,"LEDTask",4096,nullptr,1,nullptr,1)!=pdPASS)
    fatal("LEDTask creation failed");
  Serial.printf("Ready: model=%u bytes, arena=%lu bytes, period=%lu ms\n",
                (unsigned)model_data_len,(unsigned long)ARENA,(unsigned long)SAMPLE_MS);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000)); // FreeRTOS tasks own the application
}
