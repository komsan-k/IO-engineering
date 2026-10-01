# Lab — ESP32 BLE Mesh with 4 Devices Using Arduino-Style Code
## One Provisioner + Three Light Nodes

## Important Compatibility Note

This lab uses **Arduino-style `setup()` / `loop()` code**, but it calls Espressif's **ESP-BLE-MESH ESP-IDF APIs** directly.

For a normal Arduino sketch, include:

```cpp
#include "esp32-hal-alloc-ble-mem.h"
```

so Arduino does not release BLE memory before the BLE Mesh stack starts.

However, BLE Mesh also depends on ESP-IDF `sdkconfig` options. The standard Arduino IDE does not provide `menuconfig`, so the most reliable setup is:

```text
Arduino code
+
Arduino Core as an ESP-IDF component
+
CONFIG_BLE_MESH enabled
```

This keeps the familiar Arduino programming style while using the official ESP-BLE-MESH stack.

---

# 1. Network Architecture

```text
                    ESP32 #1
                   Provisioner
              Generic OnOff Client
                       |
          +------------+------------+
          |            |            |
          v            v            v
     ESP32 #2      ESP32 #3      ESP32 #4
     Light 1       Light 2       Light 3
     OnOff Server  OnOff Server  OnOff Server
         LED           LED           LED
```

All three light nodes run the same Arduino-style program.

---

# 2. Hardware

```text
4 × ESP32 boards
3 × LEDs
3 × 220 Ω resistors
4 × USB cables
```

Light-node wiring:

```text
GPIO 2 ---- 220 Ω ---- LED ---- GND
```

---

# 3. Required BLE Mesh Configuration

Enable BLE Mesh in the ESP-IDF configuration used to build Arduino:

```text
CONFIG_BT_ENABLED=y
CONFIG_BLUEDROID_ENABLED=y
CONFIG_BLE_MESH=y
CONFIG_BLE_MESH_NODE=y
CONFIG_BLE_MESH_PROVISIONER=y
```

Exact option names can vary slightly with ESP-IDF versions.

---

# 4. Light Node Sketch

Save as:

```text
BLE_Mesh_Light_Node.ino
```

Upload the same sketch to:

```text
ESP32 #2
ESP32 #3
ESP32 #4
```

## Complete Arduino-Style Light Node Code

```cpp
#include <Arduino.h>

#include "esp32-hal-alloc-ble-mem.h"

extern "C" {
  #include "esp_log.h"
  #include "esp_mac.h"

  #include "esp_bt.h"
  #include "esp_bt_main.h"

  #include "esp_ble_mesh_defs.h"
  #include "esp_ble_mesh_common_api.h"
  #include "esp_ble_mesh_networking_api.h"
  #include "esp_ble_mesh_provisioning_api.h"
  #include "esp_ble_mesh_config_model_api.h"
  #include "esp_ble_mesh_generic_model_api.h"
}

#define LED_PIN 2
#define CID_ESP 0x02E5

static const char *TAG =
  "LIGHT_NODE";


// --------------------------------
// Device UUID
// All classroom light nodes start
// with DD DD.
// --------------------------------

static uint8_t devUUID[16] =
{
  0xDD, 0xDD
};


// --------------------------------
// Configuration Server
// --------------------------------

static esp_ble_mesh_cfg_srv_t
configServer =
{
  .net_transmit =
    ESP_BLE_MESH_TRANSMIT(
      2,
      20
    ),

  .relay =
    ESP_BLE_MESH_RELAY_ENABLED,

  .relay_retransmit =
    ESP_BLE_MESH_TRANSMIT(
      2,
      20
    ),

  .beacon =
    ESP_BLE_MESH_BEACON_ENABLED,

  .gatt_proxy =
    ESP_BLE_MESH_GATT_PROXY_ENABLED,

  .friend_state =
    ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,

  .default_ttl =
    7
};


// --------------------------------
// Generic OnOff Server
// --------------------------------

ESP_BLE_MESH_MODEL_PUB_DEFINE(
  onoffPub,
  2 + 3,
  ROLE_NODE
);

static esp_ble_mesh_gen_onoff_srv_t
onoffServer =
{
  .rsp_ctrl =
  {
    .get_auto_rsp =
      ESP_BLE_MESH_SERVER_AUTO_RSP,

    .set_auto_rsp =
      ESP_BLE_MESH_SERVER_AUTO_RSP
  }
};


// --------------------------------
// Models
// --------------------------------

static esp_ble_mesh_model_t
rootModels[] =
{
  ESP_BLE_MESH_MODEL_CFG_SRV(
    &configServer
  ),

  ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(
    &onoffPub,
    &onoffServer
  )
};


// --------------------------------
// Element
// --------------------------------

static esp_ble_mesh_elem_t
elements[] =
{
  ESP_BLE_MESH_ELEMENT(
    0,
    rootModels,
    ESP_BLE_MESH_MODEL_NONE
  )
};


// --------------------------------
// Composition
// --------------------------------

static esp_ble_mesh_comp_t
composition =
{
  .cid =
    CID_ESP,

  .element_count =
    ARRAY_SIZE(
      elements
    ),

  .elements =
    elements
};


// --------------------------------
// Provisioning Data
// --------------------------------

static esp_ble_mesh_prov_t
provision =
{
  .uuid =
    devUUID
};


// ========================================
// Bluetooth Initialization
// ========================================

bool initBluetooth()
{
  esp_err_t err;

  esp_bt_controller_config_t cfg =
    BT_CONTROLLER_INIT_CONFIG_DEFAULT();

  err =
    esp_bt_controller_init(
      &cfg
    );

  if (
    err != ESP_OK
  )
  {
    Serial.println(
      "BT controller init failed"
    );

    return false;
  }


  err =
    esp_bt_controller_enable(
      ESP_BT_MODE_BLE
    );

  if (
    err != ESP_OK
  )
  {
    Serial.println(
      "BT controller enable failed"
    );

    return false;
  }


  err =
    esp_bluedroid_init();

  if (
    err != ESP_OK
  )
  {
    Serial.println(
      "Bluedroid init failed"
    );

    return false;
  }


  err =
    esp_bluedroid_enable();

  if (
    err != ESP_OK
  )
  {
    Serial.println(
      "Bluedroid enable failed"
    );

    return false;
  }


  return true;
}


// ========================================
// Create Device UUID
// ========================================

void createDeviceUUID()
{
  uint8_t mac[6];

  esp_read_mac(
    mac,
    ESP_MAC_BT
  );


  devUUID[0] =
    0xDD;

  devUUID[1] =
    0xDD;


  memcpy(
    &devUUID[2],
    mac,
    6
  );


  Serial.print(
    "Device UUID prefix: "
  );

  Serial.println(
    "DD:DD"
  );
}


// ========================================
// Provisioning Callback
// ========================================

void provisioningCallback(
  esp_ble_mesh_prov_cb_event_t event,
  esp_ble_mesh_prov_cb_param_t *param
)
{
  switch (event)
  {
    case
    ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:

      Serial.print(
        "Provisioned. Address: 0x"
      );

      Serial.println(
        param
          ->node_prov_complete
          .addr,
        HEX
      );

      break;


    case
    ESP_BLE_MESH_NODE_PROV_RESET_EVT:

      Serial.println(
        "BLE Mesh reset"
      );

      break;


    default:
      break;
  }
}


// ========================================
// Generic Server Callback
// ========================================

void genericServerCallback(
  esp_ble_mesh_generic_server_cb_event_t event,
  esp_ble_mesh_generic_server_cb_param_t *param
)
{
  if (
    event !=
    ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT
  )
  {
    return;
  }


  if (
    param->ctx.recv_op ==
      ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET
    ||
    param->ctx.recv_op ==
      ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK
  )
  {
    uint8_t state =
      param
        ->value
        .state_change
        .onoff_set
        .onoff;


    digitalWrite(
      LED_PIN,
      state
        ? HIGH
        : LOW
    );


    Serial.print(
      "LED = "
    );

    Serial.println(
      state
        ? "ON"
        : "OFF"
    );
  }
}


// ========================================
// Initialize BLE Mesh Node
// ========================================

bool initMeshNode()
{
  esp_ble_mesh_register_prov_callback(
    provisioningCallback
  );


  esp_ble_mesh_register_generic_server_callback(
    genericServerCallback
  );


  esp_err_t err =
    esp_ble_mesh_init(
      &provision,
      &composition
    );


  if (
    err != ESP_OK
  )
  {
    Serial.printf(
      "Mesh init error: %d\n",
      err
    );

    return false;
  }


  err =
    esp_ble_mesh_node_prov_enable(
      ESP_BLE_MESH_PROV_ADV |
      ESP_BLE_MESH_PROV_GATT
    );


  if (
    err != ESP_OK
  )
  {
    Serial.printf(
      "Provisioning enable error: %d\n",
      err
    );

    return false;
  }


  return true;
}


// ========================================
// Arduino Setup
// ========================================

void setup()
{
  Serial.begin(
    115200
  );

  delay(
    1000
  );


  pinMode(
    LED_PIN,
    OUTPUT
  );

  digitalWrite(
    LED_PIN,
    LOW
  );


  Serial.println();
  Serial.println(
    "ESP32 BLE Mesh Light Node"
  );


  createDeviceUUID();


  if (
    !initBluetooth()
  )
  {
    return;
  }


  if (
    !initMeshNode()
  )
  {
    return;
  }


  Serial.println(
    "Waiting for provisioning..."
  );
}


// ========================================
// Arduino Loop
// ========================================

void loop()
{
  delay(
    1000
  );
}
```

---

# 5. Provisioner Sketch

Save as:

```text
BLE_Mesh_Provisioner.ino
```

Upload to:

```text
ESP32 #1
```

## Complete Arduino-Style Provisioner Code

```cpp
#include <Arduino.h>

#include "esp32-hal-alloc-ble-mem.h"

extern "C" {
  #include "esp_log.h"
  #include "esp_mac.h"

  #include "esp_bt.h"
  #include "esp_bt_main.h"

  #include "esp_ble_mesh_defs.h"
  #include "esp_ble_mesh_common_api.h"
  #include "esp_ble_mesh_networking_api.h"
  #include "esp_ble_mesh_provisioning_api.h"
  #include "esp_ble_mesh_config_model_api.h"
  #include "esp_ble_mesh_generic_model_api.h"
}

#define CID_ESP 0x02E5

#define PROVISIONER_ADDR \
  0x0001

#define APP_IDX \
  0x0000

#define MAX_NODES \
  3

#define MESSAGE_TTL \
  3


// --------------------------------
// Provisioner UUID
// --------------------------------

static uint8_t
provUUID[16];


// --------------------------------
// Mesh Node Record
// --------------------------------

struct LightNode
{
  uint16_t address;

  bool configured;

  uint8_t state;
};


LightNode
lightNodes[MAX_NODES];

int nodeCount =
  0;


// --------------------------------
// Mesh Keys
// --------------------------------

static uint16_t
netIdx =
  ESP_BLE_MESH_KEY_PRIMARY;

static uint16_t
appIdx =
  APP_IDX;

static uint8_t
appKey[16];


// --------------------------------
// Client Models
// --------------------------------

static esp_ble_mesh_client_t
configClient;

static esp_ble_mesh_client_t
onoffClient;


// --------------------------------
// Configuration Server
// --------------------------------

static esp_ble_mesh_cfg_srv_t
configServer =
{
  .net_transmit =
    ESP_BLE_MESH_TRANSMIT(
      2,
      20
    ),

  .relay =
    ESP_BLE_MESH_RELAY_DISABLED,

  .beacon =
    ESP_BLE_MESH_BEACON_ENABLED,

  .gatt_proxy =
    ESP_BLE_MESH_GATT_PROXY_ENABLED,

  .friend_state =
    ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,

  .default_ttl =
    7
};


// --------------------------------
// Models
// --------------------------------

static esp_ble_mesh_model_t
rootModels[] =
{
  ESP_BLE_MESH_MODEL_CFG_SRV(
    &configServer
  ),

  ESP_BLE_MESH_MODEL_CFG_CLI(
    &configClient
  ),

  ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(
    NULL,
    &onoffClient
  )
};


// --------------------------------
// Element
// --------------------------------

static esp_ble_mesh_elem_t
elements[] =
{
  ESP_BLE_MESH_ELEMENT(
    0,
    rootModels,
    ESP_BLE_MESH_MODEL_NONE
  )
};


// --------------------------------
// Composition
// --------------------------------

static esp_ble_mesh_comp_t
composition =
{
  .cid =
    CID_ESP,

  .element_count =
    ARRAY_SIZE(
      elements
    ),

  .elements =
    elements
};


// --------------------------------
// Provisioner Configuration
// --------------------------------

static esp_ble_mesh_prov_t
provision =
{
  .prov_uuid =
    provUUID,

  .prov_unicast_addr =
    PROVISIONER_ADDR,

  .prov_start_address =
    0x0005,

  .prov_attention =
    0,

  .prov_algorithm =
    0,

  .prov_pub_key_oob =
    0,

  .prov_static_oob_val =
    NULL,

  .prov_static_oob_len =
    0,

  .flags =
    0,

  .iv_index =
    0
};


// ========================================
// Bluetooth Initialization
// ========================================

bool initBluetooth()
{
  esp_bt_controller_config_t cfg =
    BT_CONTROLLER_INIT_CONFIG_DEFAULT();


  if (
    esp_bt_controller_init(
      &cfg
    )
    != ESP_OK
  )
  {
    Serial.println(
      "BT init failed"
    );

    return false;
  }


  if (
    esp_bt_controller_enable(
      ESP_BT_MODE_BLE
    )
    != ESP_OK
  )
  {
    Serial.println(
      "BT enable failed"
    );

    return false;
  }


  if (
    esp_bluedroid_init()
    != ESP_OK
  )
  {
    Serial.println(
      "Bluedroid init failed"
    );

    return false;
  }


  if (
    esp_bluedroid_enable()
    != ESP_OK
  )
  {
    Serial.println(
      "Bluedroid enable failed"
    );

    return false;
  }


  return true;
}


// ========================================
// Provisioner UUID
// ========================================

void createProvisionerUUID()
{
  uint8_t mac[6];

  esp_read_mac(
    mac,
    ESP_MAC_BT
  );


  memset(
    provUUID,
    0,
    sizeof(
      provUUID
    )
  );


  provUUID[0] =
    0xAA;

  provUUID[1] =
    0xAA;


  memcpy(
    &provUUID[2],
    mac,
    6
  );
}


// ========================================
// Find Node
// ========================================

LightNode* findNode(
  uint16_t address
)
{
  for (
    int i = 0;
    i < nodeCount;
    i++
  )
  {
    if (
      lightNodes[i].address ==
      address
    )
    {
      return
        &lightNodes[i];
    }
  }


  return nullptr;
}


// ========================================
// Common Message Parameters
// ========================================

void prepareCommon(
  esp_ble_mesh_client_common_param_t *common,
  esp_ble_mesh_model_t *model,
  uint16_t destination,
  uint32_t opcode
)
{
  memset(
    common,
    0,
    sizeof(
      *common
    )
  );


  common->opcode =
    opcode;

  common->model =
    model;

  common->ctx.net_idx =
    netIdx;

  common->ctx.app_idx =
    appIdx;

  common->ctx.addr =
    destination;

  common->ctx.send_ttl =
    MESSAGE_TTL;

  common->msg_timeout =
    0;
}


// ========================================
// Request Composition Data
// ========================================

void getComposition(
  uint16_t address
)
{
  esp_ble_mesh_client_common_param_t
  common = {};


  esp_ble_mesh_cfg_client_get_state_t
  getState = {};


  prepareCommon(
    &common,
    configClient.model,
    address,
    ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET
  );


  getState.comp_data_get.page =
    0;


  esp_ble_mesh_config_client_get_state(
    &common,
    &getState
  );
}


// ========================================
// Add AppKey to Node
// ========================================

void addAppKey(
  uint16_t address
)
{
  esp_ble_mesh_client_common_param_t
  common = {};


  esp_ble_mesh_cfg_client_set_state_t
  setState = {};


  prepareCommon(
    &common,
    configClient.model,
    address,
    ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD
  );


  setState.app_key_add.net_idx =
    netIdx;

  setState.app_key_add.app_idx =
    appIdx;


  memcpy(
    setState.app_key_add.app_key,
    appKey,
    16
  );


  esp_ble_mesh_config_client_set_state(
    &common,
    &setState
  );
}


// ========================================
// Bind OnOff Server
// ========================================

void bindOnOff(
  uint16_t address
)
{
  esp_ble_mesh_client_common_param_t
  common = {};


  esp_ble_mesh_cfg_client_set_state_t
  setState = {};


  prepareCommon(
    &common,
    configClient.model,
    address,
    ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND
  );


  setState.model_app_bind.element_addr =
    address;

  setState.model_app_bind.model_app_idx =
    appIdx;

  setState.model_app_bind.model_id =
    ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;

  setState.model_app_bind.company_id =
    ESP_BLE_MESH_CID_NVAL;


  esp_ble_mesh_config_client_set_state(
    &common,
    &setState
  );
}


// ========================================
// Send ON/OFF
// ========================================

void sendOnOff(
  uint16_t address,
  uint8_t state
)
{
  static uint8_t tid =
    0;


  esp_ble_mesh_client_common_param_t
  common = {};


  esp_ble_mesh_generic_client_set_state_t
  setState = {};


  prepareCommon(
    &common,
    onoffClient.model,
    address,
    ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET
  );


  setState.onoff_set.op_en =
    false;

  setState.onoff_set.onoff =
    state;

  setState.onoff_set.tid =
    tid++;


  esp_err_t err =
    esp_ble_mesh_generic_client_set_state(
      &common,
      &setState
    );


  Serial.print(
    "Send "
  );

  Serial.print(
    state
      ? "ON"
      : "OFF"
  );

  Serial.print(
    " -> 0x"
  );

  Serial.println(
    address,
    HEX
  );


  if (
    err != ESP_OK
  )
  {
    Serial.printf(
      "Send error: %d\n",
      err
    );
  }
}


// ========================================
// Add Unprovisioned Device
// ========================================

void addUnprovisionedDevice(
  uint8_t uuid[16],
  uint8_t addr[6],
  esp_ble_mesh_addr_type_t addrType,
  uint16_t oobInfo,
  esp_ble_mesh_prov_bearer_t bearer
)
{
  if (
    nodeCount >= MAX_NODES
  )
  {
    return;
  }


  // Only use classroom nodes
  if (
    uuid[0] != 0xDD
    ||
    uuid[1] != 0xDD
  )
  {
    return;
  }


  esp_ble_mesh_unprov_dev_add_t
  device = {};


  memcpy(
    device.addr,
    addr,
    6
  );


  memcpy(
    device.uuid,
    uuid,
    16
  );


  device.addr_type =
    addrType;

  device.oob_info =
    oobInfo;

  device.bearer =
    bearer;


  esp_ble_mesh_provisioner_add_unprov_dev(
    &device,

    ADD_DEV_RM_AFTER_PROV_FLAG |
    ADD_DEV_START_PROV_NOW_FLAG |
    ADD_DEV_FLUSHABLE_DEV_FLAG
  );
}


// ========================================
// Provisioning Callback
// ========================================

void provisioningCallback(
  esp_ble_mesh_prov_cb_event_t event,
  esp_ble_mesh_prov_cb_param_t *param
)
{
  switch (event)
  {
    case
    ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:

      addUnprovisionedDevice(
        param
          ->provisioner_recv_unprov_adv_pkt
          .dev_uuid,

        param
          ->provisioner_recv_unprov_adv_pkt
          .addr,

        param
          ->provisioner_recv_unprov_adv_pkt
          .addr_type,

        param
          ->provisioner_recv_unprov_adv_pkt
          .oob_info,

        param
          ->provisioner_recv_unprov_adv_pkt
          .bearer
      );

      break;


    case
    ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
    {
      if (
        nodeCount < MAX_NODES
      )
      {
        lightNodes[nodeCount].address =
          param
            ->provisioner_prov_complete
            .unicast_addr;


        lightNodes[nodeCount].configured =
          false;


        Serial.print(
          "Provisioned Node "
        );

        Serial.print(
          nodeCount + 1
        );

        Serial.print(
          " at 0x"
        );

        Serial.println(
          lightNodes[nodeCount].address,
          HEX
        );


        uint16_t address =
          lightNodes[nodeCount].address;


        nodeCount++;


        getComposition(
          address
        );
      }

      break;
    }


    case
    ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT:

      if (
        param
          ->provisioner_add_app_key_comp
          .err_code
        ==
        ESP_OK
      )
      {
        appIdx =
          param
            ->provisioner_add_app_key_comp
            .app_idx;


        esp_ble_mesh_provisioner_bind_app_key_to_local_model(
          PROVISIONER_ADDR,
          appIdx,
          ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI,
          ESP_BLE_MESH_CID_NVAL
        );


        Serial.println(
          "Local OnOff Client bound"
        );
      }

      break;


    default:
      break;
  }
}


// ========================================
// Configuration Client Callback
// ========================================

void configClientCallback(
  esp_ble_mesh_cfg_client_cb_event_t event,
  esp_ble_mesh_cfg_client_cb_param_t *param
)
{
  if (
    param == nullptr
    ||
    param->params == nullptr
  )
  {
    return;
  }


  uint16_t address =
    param
      ->params
      ->ctx
      .addr;


  uint32_t opcode =
    param
      ->params
      ->opcode;


  if (
    param->error_code != 0
  )
  {
    Serial.printf(
      "Config error: %d\n",
      param->error_code
    );

    return;
  }


  if (
    event ==
      ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT
    &&
    opcode ==
      ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET
  )
  {
    Serial.print(
      "Composition received: 0x"
    );

    Serial.println(
      address,
      HEX
    );


    addAppKey(
      address
    );

    return;
  }


  if (
    event ==
      ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT
    &&
    opcode ==
      ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD
  )
  {
    Serial.print(
      "AppKey added: 0x"
    );

    Serial.println(
      address,
      HEX
    );


    bindOnOff(
      address
    );

    return;
  }


  if (
    event ==
      ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT
    &&
    opcode ==
      ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND
  )
  {
    LightNode *node =
      findNode(
        address
      );


    if (
      node != nullptr
    )
    {
      node->configured =
        true;


      Serial.print(
        "Node READY: 0x"
      );

      Serial.println(
        address,
        HEX
      );
    }
  }
}


// ========================================
// Generic Client Callback
// ========================================

void genericClientCallback(
  esp_ble_mesh_generic_client_cb_event_t event,
  esp_ble_mesh_generic_client_cb_param_t *param
)
{
  if (
    event ==
      ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT
  )
  {
    if (
      param != nullptr
      &&
      param->params != nullptr
    )
    {
      Serial.print(
        "Response from 0x"
      );

      Serial.println(
        param
          ->params
          ->ctx
          .addr,
        HEX
      );
    }
  }
}


// ========================================
// Initialize Provisioner
// ========================================

bool initProvisioner()
{
  memset(
    appKey,
    0x12,
    sizeof(
      appKey
    )
  );


  esp_ble_mesh_register_prov_callback(
    provisioningCallback
  );


  esp_ble_mesh_register_config_client_callback(
    configClientCallback
  );


  esp_ble_mesh_register_generic_client_callback(
    genericClientCallback
  );


  esp_err_t err =
    esp_ble_mesh_init(
      &provision,
      &composition
    );


  if (
    err != ESP_OK
  )
  {
    Serial.printf(
      "Mesh init error: %d\n",
      err
    );

    return false;
  }


  uint8_t match[2] =
  {
    0xDD,
    0xDD
  };


  err =
    esp_ble_mesh_provisioner_set_dev_uuid_match(
      match,
      2,
      0,
      false
    );


  if (
    err != ESP_OK
  )
  {
    return false;
  }


  err =
    esp_ble_mesh_provisioner_prov_enable(
      ESP_BLE_MESH_PROV_ADV |
      ESP_BLE_MESH_PROV_GATT
    );


  if (
    err != ESP_OK
  )
  {
    return false;
  }


  err =
    esp_ble_mesh_provisioner_add_local_app_key(
      appKey,
      netIdx,
      appIdx
    );


  if (
    err != ESP_OK
  )
  {
    return false;
  }


  return true;
}


// ========================================
// Arduino Setup
// ========================================

void setup()
{
  Serial.begin(
    115200
  );

  delay(
    1000
  );


  Serial.println();
  Serial.println(
    "ESP32 BLE Mesh Provisioner"
  );


  createProvisionerUUID();


  if (
    !initBluetooth()
  )
  {
    return;
  }


  if (
    !initProvisioner()
  )
  {
    Serial.println(
      "Provisioner initialization failed"
    );

    return;
  }


  Serial.println(
    "Scanning for light nodes..."
  );
}


// ========================================
// Arduino Loop
// ========================================

void loop()
{
  static unsigned long
  lastAction =
    0;


  static int nodeIndex =
    0;


  static bool state =
    true;


  if (
    millis() - lastAction
    < 2000
  )
  {
    return;
  }


  lastAction =
    millis();


  if (
    nodeCount != MAX_NODES
  )
  {
    return;
  }


  for (
    int i = 0;
    i < MAX_NODES;
    i++
  )
  {
    if (
      !lightNodes[i].configured
    )
    {
      return;
    }
  }


  sendOnOff(
    lightNodes[nodeIndex].address,
    state
  );


  nodeIndex++;


  if (
    nodeIndex >= MAX_NODES
  )
  {
    nodeIndex =
      0;

    state =
      !state;
  }
}
```

---

# 6. Expected Provisioning Sequence

The provisioner should report approximately:

```text
ESP32 BLE Mesh Provisioner
Scanning for light nodes...

Provisioned Node 1 at 0x0005
Composition received: 0x0005
AppKey added: 0x0005
Node READY: 0x0005

Provisioned Node 2 at 0x0006
Composition received: 0x0006
AppKey added: 0x0006
Node READY: 0x0006

Provisioned Node 3 at 0x0007
Composition received: 0x0007
AppKey added: 0x0007
Node READY: 0x0007
```

---

# 7. Expected Light Sequence

After all three nodes are ready:

```text
Light 1 ON
Light 2 ON
Light 3 ON

Light 1 OFF
Light 2 OFF
Light 3 OFF
```

Then the sequence repeats.

---

# 8. Expected Light-Node Monitor

```text
ESP32 BLE Mesh Light Node
Waiting for provisioning...

Provisioned. Address: 0x5

LED = ON
LED = OFF
LED = ON
LED = OFF
```

---

# 9. BLE Mesh Communication Flow

```text
Provisioner
    |
    | Provision
    v
Light Node
    |
    | Composition Data
    v
Provisioner
    |
    | AppKey Add
    v
Light Node
    |
    | Model App Bind
    v
Generic OnOff Server Ready
    |
    | OnOff SET
    v
LED
```

---

# 10. Experiment 1 — Individual Node Control

Replace the automatic sequence in `loop()` with:

```cpp
sendOnOff(
  lightNodes[0].address,
  1
);
```

Only Light Node 1 should turn ON.

---

# 11. Experiment 2 — Running Light

Use:

```text
Light 1 ON
Light 1 OFF
Light 2 ON
Light 2 OFF
Light 3 ON
Light 3 OFF
```

to create a running-light effect.

---

# 12. Experiment 3 — Relay / Multi-Hop

The light node configuration enables:

```cpp
.relay =
  ESP_BLE_MESH_RELAY_ENABLED
```

This allows suitable mesh packets to be relayed.

Conceptually:

```text
Provisioner
     |
     v
Light 1
 Relay
     |
     v
Light 2
 Relay
     |
     v
Light 3
```

---

# 13. Checkpoint Questions

1. Why is `esp32-hal-alloc-ble-mem.h` included?
2. What does the provisioner do?
3. Why do the light nodes use the UUID prefix `DD DD`?
4. What is a BLE Mesh unicast address?
5. What is an AppKey?
6. Why is the AppKey bound to the Generic OnOff Server?
7. What does the Generic OnOff Client do?
8. What does the Generic OnOff Server do?
9. What is a relay?
10. Why is this more complex than normal BLE client/server communication?

---

# 14. Simple Assignment

Modify the network so that:

```text
ESP32 #2 = Room A
ESP32 #3 = Room B
ESP32 #4 = Room C
```

Create this sequence:

```text
Room A ON
Room B ON
Room C ON
Room A OFF
Room B OFF
Room C OFF
```

Then modify the timing to create a faster running-light pattern.

---

# 15. Important Practical Note

The code above is **Arduino-style**, but true Bluetooth Mesh is still provided by the ESP-IDF BLE Mesh stack.

Therefore:

```text
Normal Arduino BLE API
BLEDevice.h
        ≠
Bluetooth Mesh
```

For BLE Mesh:

```text
Arduino setup()/loop()
        +
ESP-IDF BLE Mesh APIs
        +
BLE Mesh sdkconfig options
```

is required.

If a header such as:

```text
esp_ble_mesh_defs.h
```

is missing, or `CONFIG_BLE_MESH` is disabled, build the Arduino core as an ESP-IDF component with BLE Mesh enabled rather than attempting to install a separate Arduino BLE Mesh library.

---

## Key Takeaway

The four-device network is:

```text
Arduino-Style Application
          |
          v
ESP-BLE-MESH Stack
          |
          v
Provisioner
          |
          +---- Light Node 1
          +---- Light Node 2
          +---- Light Node 3
```

This gives students Arduino-style programming while retaining the standardized Bluetooth Mesh provisioning, addressing, model binding, security keys, and multi-hop capabilities.
