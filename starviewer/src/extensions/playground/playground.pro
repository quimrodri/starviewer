# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground
# L'objectiu �s un subdirectori del projecte

SUBDIRS += diffusionperfusionsegmentation \
           edemasegmentation \
           angiosubstraction \
           perfusionmapreconstruction \
           rectumsegmentation \
           experimental3d
include(../../extensions.inc)
TEMPLATE = subdirs
include(../../compilationtype.inc)
for(dir, SUBDIRS){
  !exists($$dir){
    SUBDIRS -= $$dir
  }
  !contains(PLAYGROUND_EXTENSIONS, $$dir){
    SUBDIRS -= $$dir
  }
}
