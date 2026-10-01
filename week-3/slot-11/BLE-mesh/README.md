# Lab — Simple ESP32 BLE Mesh with 4 Devices
## One Provisioner and Three Light Nodes — Complete ESP-IDF Code

## Objective

Create a small **Bluetooth LE Mesh** network using four ESP32 boards:

- **ESP32 #1** — Provisioner + Generic OnOff Client
- **ESP32 #2** — Light Node 1 + Generic OnOff Server
- **ESP32 #3** — Light Node 2 + Generic OnOff Server
- **ESP32 #4** — Light Node 3 + Generic OnOff Server

The provisioner automatically discovers and provisions the three light nodes, adds an AppKey, binds the Generic OnOff Server model, and then periodically sends ON/OFF commands.

> **Important:** ESP32 BLE Mesh is implemented with **ESP-IDF ESP-BLE-MESH**. This is not a normal Arduino `BLEDevice.h` sketch.

---

# 1. System Architecture

```text
                   ESP32 #1
              BLE Mesh Provisioner
              Generic OnOff Client
                      |
          +-----------+-----------+
          |           |           |
          v           v           v
      ESP32 #2    ESP32 #3    ESP32 #4
      Light 1     Light 2     Light 3
      OnOff       OnOff       OnOff
      Server      Server      Server
          |           |           |
          v           v           v
         LED         LED         LED
```

---

# 2. Hardware

```text
4 × ESP32 boards
4 × USB cables
3 × LEDs
3 × 220 Ω resistors
```

For each light node:

```text
GPIO 2 ---- 220 Ω ---- LED ---- GND
```

If your board uses another onboard-LED GPIO, change:

```c
#define LED_GPIO 2
```

---

# 3. Software

Use **ESP-IDF** with ESP-BLE-MESH enabled.

The code follows the same model structure used by the official ESP-IDF examples:

```text
bluetooth/esp_ble_mesh/provisioner
bluetooth/esp_ble_mesh/onoff_models/onoff_server
```

All three light nodes use the same server program.

---

# 4. Project Structure

Create two ESP-IDF projects.

## Provisioner project

```text
ble_mesh_provisioner/
├── CMakeLists.txt
├── sdkconfig.defaults
└── main/
    ├── CMakeLists.txt
    └── main.c
```

## Light-node project

```text
ble_mesh_light/
├── CMakeLists.txt
├── sdkconfig.defaults
└── main/
    ├── CMakeLists.txt
    └── main.c
```

---

# 5. Common Top-Level `CMakeLists.txt`

Use the following file in both projects.

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(ble_mesh_lab)
```

---

# 6. `main/CMakeLists.txt`

Use the following in both projects.

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES bt nvs_flash driver
)
```

---

# 7. `sdkconfig.defaults`

Use this basic configuration:

```text
CONFIG_BT_ENABLED=y
CONFIG_BT_BLUEDROID_ENABLED=y
CONFIG_BT_BLE_ENABLED=y
CONFIG_BLE_MESH=y
CONFIG_BLE_MESH_PROVISIONER=y
```

For the light-node project, provisioning-node support is also required by ESP-BLE-MESH configuration.

Run:

```bash
idf.py menuconfig
```

and verify:

```text
Component config
  -> Bluetooth
     -> Bluetooth enabled

Component config
  -> ESP BLE Mesh
     -> Enable BLE Mesh
```

---

# 8. Complete Light-Node Code

Save as:

```text
ble_mesh_light/main/main.c
```

Flash the same program to **ESP32 #2, ESP32 #3, and ESP32 #4**.

```c
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "esp_bt.h"
#include "esp_bt_main.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"

#define TAG "BLE_MESH_LIGHT"

#define CID_ESP 0x02E5

#define LED_GPIO 2

// --------------------------------------------------
// Device UUID
// Prefix 0xDD 0xDD lets the provisioner identify
// these laboratory light nodes.
// --------------------------------------------------

static uint8_t dev_uuid[16] = {
    0xDD, 0xDD
};


// --------------------------------------------------
// Configuration Server
// --------------------------------------------------

static esp_ble_mesh_cfg_srv_t config_server = {
    .net_transmit =
        ESP_BLE_MESH_TRANSMIT(2, 20),

    .relay =
        ESP_BLE_MESH_RELAY_ENABLED,

    .relay_retransmit =
        ESP_BLE_MESH_TRANSMIT(2, 20),

    .beacon =
        ESP_BLE_MESH_BEACON_ENABLED,

    .gatt_proxy =
        ESP_BLE_MESH_GATT_PROXY_ENABLED,

    .friend_state =
        ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,

    .default_ttl = 7,
};


// --------------------------------------------------
// Generic OnOff Server
// --------------------------------------------------

ESP_BLE_MESH_MODEL_PUB_DEFINE(
    onoff_pub,
    2 + 3,
    ROLE_NODE
);

static esp_ble_mesh_gen_onoff_srv_t
    onoff_server = {

    .rsp_ctrl = {
        .get_auto_rsp =
            ESP_BLE_MESH_SERVER_AUTO_RSP,

        .set_auto_rsp =
            ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
};


// --------------------------------------------------
// Root Models
// --------------------------------------------------

static esp_ble_mesh_model_t root_models[] = {

    ESP_BLE_MESH_MODEL_CFG_SRV(
        &config_server
    ),

    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(
        &onoff_pub,
        &onoff_server
    ),
};


// --------------------------------------------------
// Element
// --------------------------------------------------

static esp_ble_mesh_elem_t elements[] = {

    ESP_BLE_MESH_ELEMENT(
        0,
        root_models,
        ESP_BLE_MESH_MODEL_NONE
    ),
};


// --------------------------------------------------
// Composition
// --------------------------------------------------

static esp_ble_mesh_comp_t composition = {

    .cid =
        CID_ESP,

    .element_count =
        ARRAY_SIZE(elements),

    .elements =
        elements,
};


// --------------------------------------------------
// Provisioning Information
// --------------------------------------------------

static esp_ble_mesh_prov_t provision = {

    .uuid =
        dev_uuid,

    .output_size =
        0,

    .output_actions =
        0,
};


// ==================================================
// Bluetooth Initialization
// ==================================================

static esp_err_t bluetooth_init(void)
{
    esp_err_t err;

    ESP_ERROR_CHECK(
        esp_bt_controller_mem_release(
            ESP_BT_MODE_CLASSIC_BT
        )
    );

    esp_bt_controller_config_t bt_cfg =
        BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    err =
        esp_bt_controller_init(
            &bt_cfg
        );

    if (err != ESP_OK) {
        return err;
    }

    err =
        esp_bt_controller_enable(
            ESP_BT_MODE_BLE
        );

    if (err != ESP_OK) {
        return err;
    }

    err =
        esp_bluedroid_init();

    if (err != ESP_OK) {
        return err;
    }

    err =
        esp_bluedroid_enable();

    return err;
}


// ==================================================
// Create UUID from Bluetooth MAC
// ==================================================

static void create_device_uuid(void)
{
    uint8_t bt_mac[6];

    esp_read_mac(
        bt_mac,
        ESP_MAC_BT
    );

    dev_uuid[0] = 0xDD;
    dev_uuid[1] = 0xDD;

    memcpy(
        &dev_uuid[2],
        bt_mac,
        6
    );

    ESP_LOG_BUFFER_HEX(
        TAG,
        dev_uuid,
        sizeof(dev_uuid)
    );
}


// ==================================================
// Provisioning Callback
// ==================================================

static void provisioning_cb(
    esp_ble_mesh_prov_cb_event_t event,
    esp_ble_mesh_prov_cb_param_t *param
)
{
    switch (event) {

    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:

        ESP_LOGI(
            TAG,
            "Provisioning complete"
        );

        ESP_LOGI(
            TAG,
            "Address: 0x%04X",
            param
                ->node_prov_complete
                .addr
        );

        break;


    case ESP_BLE_MESH_NODE_PROV_RESET_EVT:

        ESP_LOGI(
            TAG,
            "Node reset"
        );

        break;


    default:
        break;
    }
}


// ==================================================
// Generic Server Callback
// ==================================================

static void generic_server_cb(
    esp_ble_mesh_generic_server_cb_event_t event,
    esp_ble_mesh_generic_server_cb_param_t *param
)
{
    if (
        event ==
        ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT
    )
    {
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

            gpio_set_level(
                LED_GPIO,
                state
            );

            ESP_LOGI(
                TAG,
                "LED = %s",
                state
                    ? "ON"
                    : "OFF"
            );
        }
    }
}


// ==================================================
// BLE Mesh Initialization
// ==================================================

static esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    esp_ble_mesh_register_prov_callback(
        provisioning_cb
    );

    esp_ble_mesh_register_generic_server_callback(
        generic_server_cb
    );

    err =
        esp_ble_mesh_init(
            &provision,
            &composition
        );

    if (err != ESP_OK) {
        return err;
    }

    err =
        esp_ble_mesh_node_prov_enable(
            ESP_BLE_MESH_PROV_ADV |
            ESP_BLE_MESH_PROV_GATT
        );

    return err;
}


// ==================================================
// Main
// ==================================================

void app_main(void)
{
    esp_err_t err;

    gpio_reset_pin(
        LED_GPIO
    );

    gpio_set_direction(
        LED_GPIO,
        GPIO_MODE_OUTPUT
    );

    gpio_set_level(
        LED_GPIO,
        0
    );


    // --------------------------------
    // NVS
    // --------------------------------

    err =
        nvs_flash_init();

    if (
        err ==
            ESP_ERR_NVS_NO_FREE_PAGES
        ||
        err ==
            ESP_ERR_NVS_NEW_VERSION_FOUND
    )
    {
        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        err =
            nvs_flash_init();
    }

    ESP_ERROR_CHECK(
        err
    );


    // --------------------------------
    // Bluetooth
    // --------------------------------

    ESP_ERROR_CHECK(
        bluetooth_init()
    );


    // --------------------------------
    // Device UUID
    // --------------------------------

    create_device_uuid();


    // --------------------------------
    // BLE Mesh
    // --------------------------------

    ESP_ERROR_CHECK(
        ble_mesh_init()
    );


    ESP_LOGI(
        TAG,
        "BLE Mesh Light Node ready"
    );

    ESP_LOGI(
        TAG,
        "Waiting for provisioning..."
    );
}
```

---

# 9. Light-Node Operation

At power-up:

```text
ESP32 Light Node
       |
       v
Unprovisioned Beacon
       |
       v
Wait for Provisioner
```

After provisioning and model binding:

```text
Generic OnOff SET = 1
       |
       v
GPIO 2 = HIGH
       |
       v
LED ON
```

and:

```text
Generic OnOff SET = 0
       |
       v
GPIO 2 = LOW
       |
       v
LED OFF
```

---

# 10. Complete Provisioner Code

Save as:

```text
ble_mesh_provisioner/main/main.c
```

This provisioner automatically:

1. finds devices whose UUID starts with `DD DD`;
2. provisions up to three light nodes;
3. obtains Composition Data;
4. adds an AppKey;
5. binds the AppKey to the Generic OnOff Server;
6. periodically controls the three nodes.

```c
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"

#define TAG "BLE_MESH_PROV"

#define CID_ESP 0x02E5

#define PROV_OWN_ADDR 0x0001

#define APP_KEY_IDX 0x0000

#define MAX_LIGHT_NODES 3

#define MSG_SEND_TTL 3


// --------------------------------------------------
// Provisioner UUID
// --------------------------------------------------

static uint8_t dev_uuid[16];


// --------------------------------------------------
// Node Information
// --------------------------------------------------

typedef struct {

    uint8_t uuid[16];

    uint16_t unicast;

    uint8_t elem_num;

    bool configured;

    uint8_t onoff;

} light_node_t;


static light_node_t
    nodes[MAX_LIGHT_NODES];

static int node_count = 0;


// --------------------------------------------------
// Keys
// --------------------------------------------------

static struct {

    uint16_t net_idx;

    uint16_t app_idx;

    uint8_t app_key[16];

} mesh_key;


// --------------------------------------------------
// Models
// --------------------------------------------------

static esp_ble_mesh_client_t
    config_client;

static esp_ble_mesh_client_t
    onoff_client;


static esp_ble_mesh_cfg_srv_t
    config_server = {

    .net_transmit =
        ESP_BLE_MESH_TRANSMIT(2, 20),

    .relay =
        ESP_BLE_MESH_RELAY_DISABLED,

    .beacon =
        ESP_BLE_MESH_BEACON_ENABLED,

    .gatt_proxy =
        ESP_BLE_MESH_GATT_PROXY_ENABLED,

    .friend_state =
        ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,

    .default_ttl =
        7,
};


// --------------------------------------------------
// Models
// --------------------------------------------------

static esp_ble_mesh_model_t root_models[] = {

    ESP_BLE_MESH_MODEL_CFG_SRV(
        &config_server
    ),

    ESP_BLE_MESH_MODEL_CFG_CLI(
        &config_client
    ),

    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(
        NULL,
        &onoff_client
    ),
};


// --------------------------------------------------
// Element
// --------------------------------------------------

static esp_ble_mesh_elem_t elements[] = {

    ESP_BLE_MESH_ELEMENT(
        0,
        root_models,
        ESP_BLE_MESH_MODEL_NONE
    ),
};


// --------------------------------------------------
// Composition
// --------------------------------------------------

static esp_ble_mesh_comp_t composition = {

    .cid =
        CID_ESP,

    .element_count =
        ARRAY_SIZE(elements),

    .elements =
        elements,
};


// --------------------------------------------------
// Provisioner Configuration
// --------------------------------------------------

static esp_ble_mesh_prov_t provision = {

    .prov_uuid =
        dev_uuid,

    .prov_unicast_addr =
        PROV_OWN_ADDR,

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
        0,
};


// ==================================================
// Bluetooth Initialization
// ==================================================

static esp_err_t bluetooth_init(void)
{
    esp_err_t err;

    ESP_ERROR_CHECK(
        esp_bt_controller_mem_release(
            ESP_BT_MODE_CLASSIC_BT
        )
    );

    esp_bt_controller_config_t bt_cfg =
        BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    err =
        esp_bt_controller_init(
            &bt_cfg
        );

    if (err != ESP_OK) {
        return err;
    }

    err =
        esp_bt_controller_enable(
            ESP_BT_MODE_BLE
        );

    if (err != ESP_OK) {
        return err;
    }

    err =
        esp_bluedroid_init();

    if (err != ESP_OK) {
        return err;
    }

    return
        esp_bluedroid_enable();
}


// ==================================================
// Provisioner UUID
// ==================================================

static void create_provisioner_uuid(void)
{
    uint8_t mac[6];

    esp_read_mac(
        mac,
        ESP_MAC_BT
    );

    memset(
        dev_uuid,
        0,
        sizeof(dev_uuid)
    );

    dev_uuid[0] = 0xAA;
    dev_uuid[1] = 0xAA;

    memcpy(
        &dev_uuid[2],
        mac,
        6
    );
}


// ==================================================
// Find Node by Address
// ==================================================

static light_node_t *find_node(
    uint16_t address
)
{
    for (
        int i = 0;
        i < node_count;
        i++
    )
    {
        if (
            nodes[i].unicast ==
            address
        )
        {
            return
                &nodes[i];
        }
    }

    return NULL;
}


// ==================================================
// Configure Common Message Parameters
// ==================================================

static void set_common(
    esp_ble_mesh_client_common_param_t *common,
    esp_ble_mesh_model_t *model,
    uint16_t destination,
    uint32_t opcode
)
{
    memset(
        common,
        0,
        sizeof(*common)
    );

    common->opcode =
        opcode;

    common->model =
        model;

    common->ctx.net_idx =
        mesh_key.net_idx;

    common->ctx.app_idx =
        mesh_key.app_idx;

    common->ctx.addr =
        destination;

    common->ctx.send_ttl =
        MSG_SEND_TTL;

    common->msg_timeout =
        0;
}


// ==================================================
// Get Composition Data
// ==================================================

static void request_composition_data(
    uint16_t address
)
{
    esp_ble_mesh_client_common_param_t
        common = {0};

    esp_ble_mesh_cfg_client_get_state_t
        get_state = {0};


    set_common(
        &common,
        config_client.model,
        address,
        ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET
    );


    get_state.comp_data_get.page =
        0;


    esp_err_t err =
        esp_ble_mesh_config_client_get_state(
            &common,
            &get_state
        );


    if (err != ESP_OK) {

        ESP_LOGE(
            TAG,
            "Composition Data Get failed"
        );
    }
}


// ==================================================
// Add AppKey to Node
// ==================================================

static void add_app_key_to_node(
    uint16_t address
)
{
    esp_ble_mesh_client_common_param_t
        common = {0};

    esp_ble_mesh_cfg_client_set_state_t
        set_state = {0};


    set_common(
        &common,
        config_client.model,
        address,
        ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD
    );


    set_state.app_key_add.net_idx =
        mesh_key.net_idx;

    set_state.app_key_add.app_idx =
        mesh_key.app_idx;


    memcpy(
        set_state.app_key_add.app_key,
        mesh_key.app_key,
        16
    );


    esp_ble_mesh_config_client_set_state(
        &common,
        &set_state
    );
}


// ==================================================
// Bind Generic OnOff Server
// ==================================================

static void bind_onoff_server(
    uint16_t address
)
{
    esp_ble_mesh_client_common_param_t
        common = {0};

    esp_ble_mesh_cfg_client_set_state_t
        set_state = {0};


    set_common(
        &common,
        config_client.model,
        address,
        ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND
    );


    set_state.model_app_bind.element_addr =
        address;

    set_state.model_app_bind.model_app_idx =
        mesh_key.app_idx;

    set_state.model_app_bind.model_id =
        ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;

    set_state.model_app_bind.company_id =
        ESP_BLE_MESH_CID_NVAL;


    esp_ble_mesh_config_client_set_state(
        &common,
        &set_state
    );
}


// ==================================================
// Send Generic OnOff
// ==================================================

static void send_onoff(
    uint16_t address,
    uint8_t state
)
{
    esp_ble_mesh_client_common_param_t
        common = {0};

    esp_ble_mesh_generic_client_set_state_t
        set_state = {0};

    static uint8_t tid = 0;


    set_common(
        &common,
        onoff_client.model,
        address,
        ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET
    );


    set_state.onoff_set.op_en =
        false;

    set_state.onoff_set.onoff =
        state;

    set_state.onoff_set.tid =
        tid++;


    esp_err_t err =
        esp_ble_mesh_generic_client_set_state(
            &common,
            &set_state
        );


    if (err == ESP_OK) {

        ESP_LOGI(
            TAG,
            "Send %s to 0x%04X",
            state ? "ON" : "OFF",
            address
        );
    }
}


// ==================================================
// Receive Unprovisioned Device
// ==================================================

static void handle_unprov_device(
    uint8_t uuid[16],
    uint8_t addr[6],
    esp_ble_mesh_addr_type_t addr_type,
    uint16_t oob_info,
    esp_ble_mesh_prov_bearer_t bearer
)
{
    if (
        node_count >=
        MAX_LIGHT_NODES
    )
    {
        return;
    }


    esp_ble_mesh_unprov_dev_add_t
        add_dev = {0};


    memcpy(
        add_dev.addr,
        addr,
        6
    );

    add_dev.addr_type =
        addr_type;


    memcpy(
        add_dev.uuid,
        uuid,
        16
    );


    add_dev.oob_info =
        oob_info;

    add_dev.bearer =
        bearer;


    esp_ble_mesh_provisioner_add_unprov_dev(
        &add_dev,
        ADD_DEV_RM_AFTER_PROV_FLAG |
        ADD_DEV_START_PROV_NOW_FLAG |
        ADD_DEV_FLUSHABLE_DEV_FLAG
    );
}


// ==================================================
// Provisioning Callback
// ==================================================

static void provisioning_cb(
    esp_ble_mesh_prov_cb_event_t event,
    esp_ble_mesh_prov_cb_param_t *param
)
{
    switch (event) {

    case
    ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:

        handle_unprov_device(
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
            node_count <
            MAX_LIGHT_NODES
        )
        {
            light_node_t *node =
                &nodes[node_count];


            memcpy(
                node->uuid,
                param
                    ->provisioner_prov_complete
                    .device_uuid,
                16
            );


            node->unicast =
                param
                    ->provisioner_prov_complete
                    .unicast_addr;


            node->elem_num =
                param
                    ->provisioner_prov_complete
                    .element_num;


            node->configured =
                false;


            ESP_LOGI(
                TAG,
                "Node %d provisioned at 0x%04X",
                node_count + 1,
                node->unicast
            );


            node_count++;


            request_composition_data(
                node->unicast
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
            mesh_key.app_idx =
                param
                    ->provisioner_add_app_key_comp
                    .app_idx;


            esp_ble_mesh_provisioner_bind_app_key_to_local_model(
                PROV_OWN_ADDR,
                mesh_key.app_idx,
                ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI,
                ESP_BLE_MESH_CID_NVAL
            );
        }

        break;


    default:
        break;
    }
}


// ==================================================
// Configuration Client Callback
// ==================================================

static void config_client_cb(
    esp_ble_mesh_cfg_client_cb_event_t event,
    esp_ble_mesh_cfg_client_cb_param_t *param
)
{
    if (
        param == NULL
        ||
        param->params == NULL
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
        param->error_code
        != 0
    )
    {
        ESP_LOGE(
            TAG,
            "Config error 0x%02X",
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
        ESP_LOGI(
            TAG,
            "Composition received from 0x%04X",
            address
        );

        add_app_key_to_node(
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
        ESP_LOGI(
            TAG,
            "AppKey added to 0x%04X",
            address
        );

        bind_onoff_server(
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
        light_node_t *node =
            find_node(
                address
            );


        if (node != NULL) {

            node->configured =
                true;

            ESP_LOGI(
                TAG,
                "Node 0x%04X READY",
                address
            );
        }
    }
}


// ==================================================
// Generic Client Callback
// ==================================================

static void generic_client_cb(
    esp_ble_mesh_generic_client_cb_event_t event,
    esp_ble_mesh_generic_client_cb_param_t *param
)
{
    if (
        param == NULL
        ||
        param->params == NULL
    )
    {
        return;
    }


    if (
        event ==
        ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT
    )
    {
        ESP_LOGI(
            TAG,
            "OnOff response from 0x%04X",
            param
                ->params
                ->ctx
                .addr
        );
    }
}


// ==================================================
// Light-Control Task
// ==================================================

static void control_task(
    void *parameter
)
{
    while (1)
    {
        if (
            node_count ==
            MAX_LIGHT_NODES
        )
        {
            bool all_ready =
                true;


            for (
                int i = 0;
                i < node_count;
                i++
            )
            {
                if (
                    !nodes[i].configured
                )
                {
                    all_ready =
                        false;
                }
            }


            if (all_ready)
            {
                // --------------------------------
                // Sequential ON
                // --------------------------------

                for (
                    int i = 0;
                    i < node_count;
                    i++
                )
                {
                    send_onoff(
                        nodes[i].unicast,
                        1
                    );

                    vTaskDelay(
                        pdMS_TO_TICKS(
                            1500
                        )
                    );
                }


                vTaskDelay(
                    pdMS_TO_TICKS(
                        2000
                    )
                );


                // --------------------------------
                // Sequential OFF
                // --------------------------------

                for (
                    int i = 0;
                    i < node_count;
                    i++
                )
                {
                    send_onoff(
                        nodes[i].unicast,
                        0
                    );

                    vTaskDelay(
                        pdMS_TO_TICKS(
                            1500
                        )
                    );
                }
            }
        }


        vTaskDelay(
            pdMS_TO_TICKS(
                3000
            )
        );
    }
}


// ==================================================
// BLE Mesh Initialization
// ==================================================

static esp_err_t ble_mesh_init(void)
{
    uint8_t uuid_match[2] = {
        0xDD,
        0xDD
    };


    mesh_key.net_idx =
        ESP_BLE_MESH_KEY_PRIMARY;

    mesh_key.app_idx =
        APP_KEY_IDX;


    memset(
        mesh_key.app_key,
        0x12,
        sizeof(mesh_key.app_key)
    );


    esp_ble_mesh_register_prov_callback(
        provisioning_cb
    );

    esp_ble_mesh_register_config_client_callback(
        config_client_cb
    );

    esp_ble_mesh_register_generic_client_callback(
        generic_client_cb
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
        return err;
    }


    err =
        esp_ble_mesh_provisioner_set_dev_uuid_match(
            uuid_match,
            sizeof(uuid_match),
            0,
            false
        );


    if (
        err != ESP_OK
    )
    {
        return err;
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
        return err;
    }


    err =
        esp_ble_mesh_provisioner_add_local_app_key(
            mesh_key.app_key,
            mesh_key.net_idx,
            mesh_key.app_idx
        );


    return err;
}


// ==================================================
// Main
// ==================================================

void app_main(void)
{
    esp_err_t err;


    // --------------------------------
    // NVS
    // --------------------------------

    err =
        nvs_flash_init();


    if (
        err ==
            ESP_ERR_NVS_NO_FREE_PAGES
        ||
        err ==
            ESP_ERR_NVS_NEW_VERSION_FOUND
    )
    {
        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        err =
            nvs_flash_init();
    }


    ESP_ERROR_CHECK(
        err
    );


    // --------------------------------
    // Bluetooth
    // --------------------------------

    ESP_ERROR_CHECK(
        bluetooth_init()
    );


    // --------------------------------
    // UUID
    // --------------------------------

    create_provisioner_uuid();


    // --------------------------------
    // BLE Mesh
    // --------------------------------

    ESP_ERROR_CHECK(
        ble_mesh_init()
    );


    ESP_LOGI(
        TAG,
        "BLE Mesh Provisioner ready"
    );


    xTaskCreate(
        control_task,
        "light_control",
        4096,
        NULL,
        5,
        NULL
    );
}
```

---

# 11. Build and Flash the Light Nodes

Open:

```bash
cd ble_mesh_light
```

Then:

```bash
idf.py set-target esp32
idf.py menuconfig
idf.py build
```

Flash Light Node 1:

```bash
idf.py -p COM5 flash monitor
```

Flash Light Node 2:

```bash
idf.py -p COM6 flash monitor
```

Flash Light Node 3:

```bash
idf.py -p COM7 flash monitor
```

---

# 12. Build and Flash the Provisioner

```bash
cd ble_mesh_provisioner
```

Then:

```bash
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p COM4 flash monitor
```

---

# 13. Expected Provisioning Sequence

```text
Provisioner ready
      |
      v
Find Light Node 1
      |
      v
Provision Node 1
      |
      v
Composition Data
      |
      v
Add AppKey
      |
      v
Bind Generic OnOff Server
      |
      v
Node 1 READY
```

The same sequence repeats for Nodes 2 and 3.

Example:

```text
Node 1 provisioned at 0x0005
Node 0x0005 READY

Node 2 provisioned at 0x0006
Node 0x0006 READY

Node 3 provisioned at 0x0007
Node 0x0007 READY
```

---

# 14. Expected LED Sequence

When all three nodes are configured:

```text
Light 1 ON
      |
      v
Light 2 ON
      |
      v
Light 3 ON
      |
      v
Wait
      |
      v
Light 1 OFF
      |
      v
Light 2 OFF
      |
      v
Light 3 OFF
```

The sequence repeats.

---

# 15. Expected Provisioner Monitor

```text
BLE Mesh Provisioner ready

Node 1 provisioned at 0x0005
Composition received from 0x0005
AppKey added to 0x0005
Node 0x0005 READY

Node 2 provisioned at 0x0006
Composition received from 0x0006
AppKey added to 0x0006
Node 0x0006 READY

Node 3 provisioned at 0x0007
Composition received from 0x0007
AppKey added to 0x0007
Node 0x0007 READY

Send ON to 0x0005
Send ON to 0x0006
Send ON to 0x0007

Send OFF to 0x0005
Send OFF to 0x0006
Send OFF to 0x0007
```

---

# 16. Expected Light-Node Monitor

Example:

```text
BLE Mesh Light Node ready
Waiting for provisioning...

Provisioning complete
Address: 0x0005

LED = ON
LED = OFF
LED = ON
LED = OFF
```

---

# 17. Experiment 1 — Individual Control

Modify `control_task()` so that only one address is controlled.

Example:

```c
send_onoff(
    nodes[0].unicast,
    1
);
```

Expected:

```text
Light Node 1 -> ON
Light Node 2 -> unchanged
Light Node 3 -> unchanged
```

---

# 18. Experiment 2 — Sequential Light Pattern

Create:

```text
Light 1 ON
Light 1 OFF

Light 2 ON
Light 2 OFF

Light 3 ON
Light 3 OFF
```

This demonstrates node-specific unicast addressing.

---

# 19. Experiment 3 — Relay / Multi-Hop

The light-node configuration enables relay:

```c
.relay =
    ESP_BLE_MESH_RELAY_ENABLED
```

Conceptually:

```text
Provisioner
     |
     v
Light Node 1
   Relay
     |
     v
Light Node 2
   Relay
     |
     v
Light Node 3
```

A relayed message has its TTL reduced at each hop.

---

# 20. Important BLE Mesh Concepts

| Concept | Purpose |
|---|---|
| Provisioner | Adds devices to the mesh |
| Unprovisioned Device | Device not yet part of the mesh |
| NetKey | Network-layer security key |
| AppKey | Application-model security key |
| Unicast Address | Identifies one mesh element |
| Generic OnOff Client | Sends ON/OFF messages |
| Generic OnOff Server | Receives ON/OFF messages |
| Relay | Retransmits eligible mesh packets |
| TTL | Limits forwarding distance |

---

# 21. BLE Mesh Flow

```text
Unprovisioned Light
        |
        v
Provisioner
        |
        v
Assign Unicast Address
        |
        v
Add AppKey
        |
        v
Bind OnOff Server
        |
        v
Configured Light Node
        |
        v
Generic OnOff SET
        |
        v
LED ON / OFF
```

---

# 22. Checkpoint Questions

1. What is the role of the BLE Mesh provisioner?
2. Why do the light-node UUIDs start with `DD DD`?
3. What is a unicast address?
4. What is an AppKey?
5. Why is the AppKey bound to the Generic OnOff Server?
6. What model sends the ON/OFF command?
7. What model receives the ON/OFF command?
8. What does the relay feature do?
9. What is TTL?
10. Why does each light node receive a different unicast address?

---

# 23. Simple Assignment

Modify the four-device BLE Mesh so that:

```text
ESP32 #1 -> Provisioner

ESP32 #2 -> Room A Light
ESP32 #3 -> Room B Light
ESP32 #4 -> Room C Light
```

Create the following sequence:

```text
Room A ON
      |
      v
Room B ON
      |
      v
Room C ON
      |
      v
All OFF
```

Then modify the delay values to produce a running-light effect.

---

# 24. Important Version Note

ESP-BLE-MESH APIs can change between ESP-IDF minor releases. If a compiler error appears, compare the callback structures and configuration options with the `provisioner` and `onoff_server` examples shipped with the exact ESP-IDF version installed on your computer.

For a first classroom test, using an ESP-IDF release branch consistently on all computers is recommended.

---

## Key Takeaway

The complete four-device BLE Mesh system performs:

```text
Automatic Discovery
      +
Provisioning
      +
Address Assignment
      +
AppKey Configuration
      +
Model Binding
      +
Generic OnOff Messaging
      +
Relay / Multi-Hop Capability
```

This is a useful foundation for smart-lighting, building automation, and distributed BLE sensor/control networks.
