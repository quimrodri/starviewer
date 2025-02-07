
SOURCES += $$PWD/test_image.cpp \
           $$PWD/test_imageplane.cpp \
           $$PWD/test_imageorientation.cpp \
           $$PWD/test_dicomvaluerepresentationconverter.cpp \
           $$PWD/test_machineidentifier.cpp \
           $$PWD/test_patientorientation.cpp \
           $$PWD/test_anatomicalplane.cpp \
           $$PWD/test_imageorientationoperationsmapper.cpp \
           $$PWD/test_volume.cpp \
           $$PWD/test_q2dviewer.cpp \
           $$PWD/test_machineinformation.cpp \
           $$PWD/test_opacitytransferfunction.cpp \
           $$PWD/test_series.cpp \
           $$PWD/test_study.cpp \
           $$PWD/test_dicomsource.cpp \
           $$PWD/test_patient.cpp \
           $$PWD/test_mathtools.cpp \
           $$PWD/test_hangingprotocol.cpp \
           $$PWD/test_imageoverlay.cpp \
           $$PWD/test_imageoverlayreader.cpp \
           $$PWD/test_drawerbitmap.cpp \
           $$PWD/test_displayshutter.cpp \
           $$PWD/test_firewallaccesstest.cpp \
           $$PWD/test_dicomtagreader.cpp \
           $$PWD/test_patientfillerinput.cpp \
           $$PWD/test_automaticsynchronizationtooldata.cpp \
           $$PWD/test_volumepixeldata.cpp \
           $$PWD/test_drawertext.cpp \
           $$PWD/test_applicationupdatechecker.cpp \
           $$PWD/test_commandlineoption.cpp \
           $$PWD/test_applicationversiontest.cpp \
           $$PWD/test_imageoverlayregionfinder.cpp \
           $$PWD/test_hangingprotocolmanager.cpp \
           $$PWD/test_systemrequerimentstest.cpp \
           $$PWD/test_drawerpolygon.cpp \
           $$PWD/test_drawerline.cpp \
           $$PWD/test_diagnosistestresultwriter.cpp \
           $$PWD/test_vector3float.cpp \
           $$PWD/test_vector3double.cpp \
           $$PWD/test_windowlevel.cpp \
           $$PWD/test_windowlevelpresetstooldata.cpp \
           $$PWD/test_dicomformattedvaluesconverter.cpp \
           $$PWD/test_screen.cpp \
           $$PWD/test_screenlayout.cpp \
           $$PWD/test_viewerslayout.cpp \
           $$PWD/test_studylayoutconfigsettingsconverter.cpp \
           $$PWD/test_optimalviewersgridestimator.cpp \
           $$PWD/test_vtkimagedatacreator.cpp \
           $$PWD/test_pixelspacing2d.cpp \
           $$PWD/test_imagefillerstep.cpp \
           $$PWD/test_temporaldimensionfillerstep.cpp \
           $$PWD/test_computezspacingpostprocessor.cpp \
           $$PWD/test_pixelspacingamenderpostprocessor.cpp \
           $$PWD/test_volumepixeldatareaderfactory.cpp \
           $$PWD/test_measurementmanager.cpp \
           $$PWD/test_preferredpixelspacingselector.cpp \
           $$PWD/test_defaultmeasurementtypeselector.cpp \
           $$PWD/test_distancemeasurecomputer.cpp \
           $$PWD/test_areameasurecomputer.cpp \
           $$PWD/test_colortransferfunction.cpp \
           $$PWD/test_volumepixeldataiterator.cpp \
           $$PWD/test_patientcomparer.cpp \
           $$PWD/test_syncactionsconfiguration.cpp \
           $$PWD/test_dicomserviceresponsestatus.cpp \
           $$PWD/test_vtkdcmtkbydefaultvolumepixeldatareaderselector.cpp \
           $$PWD/test_itkgdcmbydefaultvolumepixeldatareaderselector.cpp \
           $$PWD/test_filter.cpp \
           $$PWD/test_filteroutput.cpp \
           $$PWD/test_orthogonalplane.cpp \
           $$PWD/test_slicehandler.cpp \
           $$PWD/test_voxel.cpp \
           $$PWD/test_roidata.cpp \
           $$PWD/test_mammographyimagehelper.cpp \
           $$PWD/test_transferfunction.cpp \
           $$PWD/test_leanbodymassformula.cpp \
           $$PWD/test_bodysurfaceareaformula.cpp \
           $$PWD/test_decaycorrectionfactorformula.cpp \
           $$PWD/test_leanbodymassformulacalculator.cpp \
           $$PWD/test_bodysurfaceareaformulacalculator.cpp \
           $$PWD/test_decaycorrectionfactorformulacalculator.cpp \
           $$PWD/test_standardizeduptakevalueleanbodymassformulacalculator.cpp \
           $$PWD/test_standardizeduptakevaluebodyweightformulacalculator.cpp \
           $$PWD/test_standardizeduptakevaluebodysurfaceareaformulacalculator.cpp

win32 {
    SOURCES += $$PWD/test_windowsfirewallaccess.cpp \
               $$PWD/test_windowssysteminformation.cpp
}
