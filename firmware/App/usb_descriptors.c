#include "tusb.h"
#include "string.h"

#define USB_PID   0x4006
#define USB_VID   0xCAFE
#define USB_BCD   0x0200

#define EPNUM_VENDOR_OUT  0x01
#define EPNUM_VENDOR_IN   0x81

#define CONFIG_TOTAL_LENGTH (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)

static tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,

    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void)
{
    return (uint8_t const*) &desc_device;
}


uint8_t const desc_configuration[] =
{
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LENGTH, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_VENDOR_DESCRIPTOR(0, 0, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, 64),
};


uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    return desc_configuration;
}

enum
{
    STRID_LANGID,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL
};

static const char* string_desc_arr[] =
{
    NULL,
    "STM32",
    "STM32G474 COT DAQ",
    "000001"
};


uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void) langid;

    uint8_t chr_count;

    if (index == STRID_LANGID)
    {
        _desc_str[1] = 0x0409;
        chr_count = 1;
    }
    else
    {
        if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))
            return NULL;

        const char* str = string_desc_arr[index];

        chr_count = strlen(str);

        for(uint8_t i=0; i<chr_count; i++)
        {
            _desc_str[1+i] = str[i];
        }
    }

    _desc_str[0] =
        (TUSB_DESC_STRING << 8) |
        (2*chr_count + 2);

    return _desc_str;
}
