#ifndef _UCA_PCOME4_CAMERA_H_
#define _UCA_PCOME4_CAMERA_H_

#include <glib-object.h>
#include <uca/uca-camera.h>

G_BEGIN_DECLS

#define UCA_TYPE_PCO_ME4_CAMERA (uca_pco_me4_camera_get_type())
#define UCA_PCO_ME4_CAMERA(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), UCA_TYPE_PCO_ME4_CAMERA, UcaPcoMe4Camera))
#define UCA_IS_PCO_ME4_CAMERA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), UCA_TYPE_PCO_ME4_CAMERA))
#define UCA_PCO_ME4_CAMERA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), UCA_TYPE_PCO_ME4_CAMERA, UcaPcoMe4CameraClass))
#define UCA_IS_PCO_ME4_CAMERA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), UCA_TYPE_PCO_ME4_CAMERA))
#define UCA_PCO_ME4_CAMERA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), UCA_TYPE_PCO_ME4_CAMERA, UcaPcoMe4CameraClass))

#define UCA_PCO_ME4_CAMERA_ERROR uca_pco_me4_camera_error_quark()

typedef enum {
    UCA_PCO_ME4_CAMERA_ERROR_PCOSDK_INIT = 1,
    UCA_PCO_ME4_CAMERA_ERROR_PCOSDK_GENERAL,
    UCA_PCO_ME4_CAMERA_ERROR_UNSUPPORTED,
    UCA_PCO_ME4_CAMERA_ERROR_FG_INIT,
    UCA_PCO_ME4_CAMERA_ERROR_FG_GENERAL,
    UCA_PCO_ME4_CAMERA_ERROR_FG_ACQUISITION
} UcaPcoMe4CameraError;

typedef enum {
    UCA_PCO_ME4_CAMERA_ACQUIRE_MODE_AUTO,
    UCA_PCO_ME4_CAMERA_ACQUIRE_MODE_EXTERNAL
} UcaPcoMe4CameraAcquireMode;

typedef enum {
    UCA_PCO_ME4_CAMERA_TIMESTAMP_NONE,
    UCA_PCO_ME4_CAMERA_TIMESTAMP_BINARY,
    UCA_PCO_ME4_CAMERA_TIMESTAMP_BOTH,
    UCA_PCO_ME4_CAMERA_TIMESTAMP_ASCII
} UcaPcoMe4CameraTimestamp;

typedef enum {
    UCA_PCO_ME4_CAMERA_STORAGE_MODE_RECORDER,
    UCA_PCO_ME4_CAMERA_STORAGE_MODE_FIFO_BUFFER
} UcaPcoMe4CameraStorageMode;
//

typedef enum {
    UCA_PCO_ME4_CAMERA_RECORD_MODE_SEQUENCE,
    UCA_PCO_ME4_CAMERA_RECORD_MODE_RING_BUFFER
} UcaPcoMe4CameraRecordMode;

typedef struct _UcaPcoMe4Camera UcaPcoMe4Camera;
typedef struct _UcaPcoMe4CameraClass UcaPcoMe4CameraClass;
typedef struct _UcaPcoMe4CameraPrivate UcaPcoMe4CameraPrivate;

/**
 * UcaPcoMe4Camera:
 *
 * Creates #UcaPcoMe4Camera instances by loading corresponding shared objects. The
 * contents of the #UcaPcoMe4Camera structure are private and should only be
 * accessed via the provided API.
 */
struct _UcaPcoMe4Camera {
    /*< private >*/
    UcaCamera parent;

    UcaPcoMe4CameraPrivate *priv;
};

/**
 * UcaPcoMe4CameraClass:
 *
 * #UcaPcoMe4Camera class
 */
struct _UcaPcoMe4CameraClass {
    /*< private >*/
    UcaCameraClass parent;
};

GType uca_pco_me4_camera_get_type(void);

G_END_DECLS

#endif /*_UCA_PCOME4_CAMERA_H_*/