
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "our_service.h"
#include "ble_srv_common.h"
#include "app_error.h"

// ALREADY_DONE_FOR_YOU: Declaration of a function that will take care of some housekeeping of ble connections related to our service and characteristic
void ble_our_service_on_ble_evt(ble_os_t * p_our_service, ble_evt_t * p_ble_evt)
{
    // OUR_JOB: Step 3.D Implement switch case handling BLE events related to our service. 
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
static uint32_t our_char_add(ble_os_t * p_our_service)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    // OUR_JOB: Step 2.A, Add a custom characteristic UUID
    
    // OUR_JOB: Step 2.F Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));

    
    // OUR_JOB: Step 3.A, Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
   
    
    // OUR_JOB: Step 2.B, Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));    
    
    
    // OUR_JOB: Step 2.G, Set read/write security levels to our characteristic
    //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    
    // OUR_JOB: Step 2.C, Configure the characteristic value attribute
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    
    // OUR_JOB: Step 2.H, Set characteristic length in number of bytes

    // OUR_JOB: Step 2.E, Add our new characteristic to the service


    return NRF_SUCCESS;
}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void our_service_init(ble_os_t * p_our_service)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    // OUR_JOB: Step 3.B, Set our service connection handle to default value. I.e. an invalid handle since we are not yet in a connection.
    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // FROM_SERVICE_TUTORIAL: Add our service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_our_service->service_handle);
    
    APP_ERROR_CHECK(err_code);
    
    // OUR_JOB: Call the function our_char_add() to add our new characteristic to the service. 
    //our_char_add(p_our_service);
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
void our_termperature_characteristic_update(ble_os_t *p_our_service, int32_t *temperature_value)
{
    // OUR_JOB: Step 3.x, Update characteristic value
    
    // Decleare variable to hold previous temperature value 
    static int32_t previous_temperature_value = 0;
    
    // If new temperature value is different from previous value then send notification
    if(*temperature_value != previous_temperature_value )
    {
        // Send value if connected and notifying
        if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
        {
            uint16_t               len = 4;
            ble_gatts_hvx_params_t hvx_params;
            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle = p_our_service->char_handles.value_handle;
            hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset = 0;
            hvx_params.p_len  = &len;
            hvx_params.p_data = (uint8_t*)temperature_value;  

            sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
        }  
    }   
    
    // Store current temperature value until next updated. 
    previous_temperature_value = *temperature_value;
    
}
