message("Init translation")

# Ensure translation directory exists
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/translation)

# Use absolute paths for translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_ar_AR.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_bn_BN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_de_DE.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_es_ES.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_fa_FA.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_fr_FR.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_he_IL.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_id_ID.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_it_IT.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_ja_JP.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_ko_KR.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_pl_PL.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_pt_BR.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_pt_PT.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_ru_RU.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_sv_SE.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_tr_TR.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_uk_UA.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_vi_VN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translation/${X_ORIGINAL_FILENAME}_zh_TW.ts
)
