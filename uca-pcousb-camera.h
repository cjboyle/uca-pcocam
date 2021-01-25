#ifndef _UCA_PCOUSB_CAMERA_H_
#define _UCA_PCOUSB_CAMERA_H_

#include <glib-object.h>
#include <uca/uca-camera.h>

G_BEGIN_DECLS

#define UCA_TYPE_PCO_USB_CAMERA (uca_pco_usb_camera_get_type())
#define UCA_PCO_USB_CAMERA(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), UCA_TYPE_PCO_USB_CAMERA, UcaPcoUsbCamera))
#define UCA_IS_PCO_USB_CAMERA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), UCA_TYPE_PCO_USB_CAMERA))
#define UCA_PCO_USB_CAMERA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), UCA_TYPE_PCO_USB_CAMERA, UcaPcoUsbCameraClass))
#define UCA_IS_PCO_USB_CAMERA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), UCA_TYPE_PCO_USB_CAMERA))
#define UCA_PCO_USB_CAMERA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), UCA_TYPE_PCO_USB_CAMERA, UcaPcoUsbCameraClass))

#define UCA_PCO_USB_CAMERA_ERROR uca_pco_usb_camera_error_quark()

typedef enum {
    UCA_PCO_USB_CAMERA_ERROR_PCOSDK_INIT = 1,
    UCA_PCO_USB_CAMERA_ERROR_PCOSDK_GENERAL,
    UCA_PCO_USB_CAMERA_ERROR_UNSUPPORTED,
    UCA_PCO_USB_CAMERA_ERROR_FG_INIT,
    UCA_PCO_USB_CAMERA_ERROR_FG_GENERAL,
    UCA_PCO_USB_CAMERA_ERROR_FG_ACQUISITION
} UcaPcoUsbCameraError;

typedef enum {
    UCA_PCO_USB_CAMERA_ACQUIRE_MODE_AUTO,
    UCA_PCO_USB_CAMERA_ACQUIRE_MODE_EXTERNAL
} UcaPcoUsbCameraAcquireMode;

typedef enum {
    UCA_PCO_USB_CAMERA_TIMESTAMP_NONE,
    UCA_PCO_USB_CAMERA_TIMESTAMP_BINARY,
    UCA_PCO_USB_CAMERA_TIMESTAMP_BOTH,
    UCA_PCO_USB_CAMERA_TIMESTAMP_ASCII
} UcaPcoUsbCameraTimestamp;

typedef struct _UcaPcoUsbCamera UcaPcoUsbCamera;
typedef struct _UcaPcoUsbCameraClass UcaPcoUsbCameraClass;
typedef struct _UcaPcoUsbCameraPrivate UcaPcoUsbCameraPrivate;

/**
 * UcaPcoUsbCamera:
 *
 * Creates #UcaPcoUsbCamera instances by loading corresponding shared objects. The
 * contents of the #UcaPcoUsbCamera structure are private and should only be
 * accessed via the provided API.
 */
struct _UcaPcoUsbCamera {
    /*< private >*/
    UcaCamera parent;

    UcaPcoUsbCameraPrivate *priv;
};

/**
 * UcaPcoUsbCameraClass:
 *
 * #UcaPcoUsbCamera class
 */
struct _UcaPcoUsbCameraClass {
    /*< private >*/
    UcaCameraClass parent;
};

GType uca_pco_usb_camera_get_type(void);

G_END_DECLS

#endif /*_UCA_PCOUSB_CAMERA_H_*/