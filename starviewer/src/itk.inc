include(defaultdirectories.inc)

INCLUDEPATH += $${ITKINCLUDEDIR} \
              $${ITKINCLUDEDIR}/Algorithms \
              $${ITKINCLUDEDIR}/BasicFilters \
              $${ITKINCLUDEDIR}/Common \
              $${ITKINCLUDEDIR}/Numerics \
              $${ITKINCLUDEDIR}/IO \
              $${ITKINCLUDEDIR}/Numerics/FEM \
              $${ITKINCLUDEDIR}/Numerics/Statistics \
              $${ITKINCLUDEDIR}/SpatialObject \
              $${ITKINCLUDEDIR}/Utilities/MetaIO \
              $${ITKINCLUDEDIR}/Utilities \
              $${ITKINCLUDEDIR}/Utilities/vxl/vcl \
              $${ITKINCLUDEDIR}/Utilities/vxl/core \
              $${ITKINCLUDEDIR}/Utilities/NrrdIO

LIBS += -L$${ITKLIBDIR} \
        -lITKAlgorithms \
        -lITKBasicFilters \
        -lITKCommon \
        -lITKDICOMParser \
        -lITKEXPAT \
        -lITKFEM \
        -lITKIO \
        -lITKMetaIO \
        -lITKNrrdIO \
        -lITKNumerics \
        -lITKSpatialObject \
        -lITKStatistics \
        -litkjpeg12 \
        -litkjpeg16 \
        -litkjpeg8 \
        -litksys \
        -litkvcl \
        -litkvnl \
        -litkvnl_algo \
        -litkvnl_inst \
        -lITKniftiio \
        -litkopenjpeg \
        -lITKznz

win32 {
 LIBS += -litktiff \
         -litkpng \
         -litkv3p_netlib \
         -litkzlib
}
