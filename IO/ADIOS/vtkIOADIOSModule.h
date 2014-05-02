
#ifndef VTKIOADIOS_EXPORT_H
#define VTKIOADIOS_EXPORT_H

#ifdef VTKIOADIOS_STATIC_DEFINE
#  define VTKIOADIOS_EXPORT
#  define VTKIOADIOS_NO_EXPORT
#else
#  ifndef VTKIOADIOS_EXPORT
#    ifdef vtkIOADIOS_EXPORTS
        /* We are building this library */
#      define VTKIOADIOS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define VTKIOADIOS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef VTKIOADIOS_NO_EXPORT
#    define VTKIOADIOS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef VTKIOADIOS_DEPRECATED
#  define VTKIOADIOS_DEPRECATED __attribute__ ((__deprecated__))
#  define VTKIOADIOS_DEPRECATED_EXPORT VTKIOADIOS_EXPORT __attribute__ ((__deprecated__))
#  define VTKIOADIOS_DEPRECATED_NO_EXPORT VTKIOADIOS_NO_EXPORT __attribute__ ((__deprecated__))
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define VTKIOADIOS_NO_DEPRECATED
#endif



#endif
