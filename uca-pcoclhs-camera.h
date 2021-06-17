#ifndef _UCA_PCOCLHS_CAMERA_H_
#define _UCA_PCOCLHS_CAMERA_H_

#include <glib-object.h>
#include <uca/uca-camera.h>

G_BEGIN_DECLS

#define UCA_TYPE_PCO_CLHS_CAMERA (uca_pco_clhs_camera_get_type())
#define UCA_PCO_CLHS_CAMERA(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), UCA_TYPE_PCO_CLHS_CAMERA, UcaPcoClhsCamera))
#define UCA_IS_PCO_CLHS_CAMERA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), UCA_TYPE_PCO_CLHS_CAMERA))
#define UCA_PCO_CLHS_CAMERA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), UCA_TYPE_PCO_CLHS_CAMERA, UcaPcoClhsCameraClass))
#define UCA_IS_PCO_CLHS_CAMERA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), UCA_TYPE_PCO_CLHS_CAMERA))
#define UCA_PCO_CLHS_CAMERA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), UCA_TYPE_PCO_CLHS_CAMERA, UcaPcoClhsCameraClass))

#define UCA_PCO_CLHS_CAMERA_ERROR uca_pco_clhs_camera_error_quark()

typedef enum {
    UCA_PCO_CLHS_CAMERA_ERROR_INIT = 1,
    UCA_PCO_CLHS_CAMERA_ERROR_GENERAL,
    UCA_PCO_CLHS_CAMERA_ERROR_UNSUPPORTED,
    UCA_PCO_CLHS_CAMERA_ERROR_TIMEOUT
} UcaPcoClhsCameraError;

typedef enum {
    UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_AUTO,
    UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_EXTERNAL
} UcaPcoClhsCameraAcquireMode;

typedef enum {
    UCA_PCO_CLHS_CAMERA_TIMESTAMP_NONE,
    UCA_PCO_CLHS_CAMERA_TIMESTAMP_BINARY,
    UCA_PCO_CLHS_CAMERA_TIMESTAMP_BOTH,
    UCA_PCO_CLHS_CAMERA_TIMESTAMP_ASCII
} UcaPcoClhsCameraTimestamp;

typedef enum {
    UCA_PCO_CLHS_CAMERA_RECORD_MODE_SEQUENCE,
    UCA_PCO_CLHS_CAMERA_RECORD_MODE_RING_BUFFER
} UcaPcoClhsCameraRecordMode;

typedef struct _UcaPcoClhsCamera UcaPcoClhsCamera;
typedef struct _UcaPcoClhsCameraClass UcaPcoClhsCameraClass;
typedef struct _UcaPcoClhsCameraPrivate UcaPcoClhsCameraPrivate;

/**
 * UcaPcoClhsCamera:
 *
 * Creates #UcaPcoClhsCamera instances by loading corresponding shared objects. The
 * contents of the #UcaPcoClhsCamera structure are private and should only be
 * accessed via the provided API.
 */
struct _UcaPcoClhsCamera {
    /*< private >*/
    UcaCamera parent;

    UcaPcoClhsCameraPrivate *priv;
};

/**
 * UcaPcoClhsCameraClass:
 *
 * #UcaPcoClhsCamera class
 */
struct _UcaPcoClhsCameraClass {
    /*< private >*/
    UcaCameraClass parent;
};

GType uca_pco_clhs_camera_get_type(void);

G_END_DECLS

#endif /*_UCA_PCOCLHS_CAMERA_H_*/