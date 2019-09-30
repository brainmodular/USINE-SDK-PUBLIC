#!/bin/sh
echo ---------------------------------
echo ------ post build script --------
echo ---------------------------------

NewName=$PRODUCT_NAME"_"$MODULE_USER_VERSION"_"$MODULE_DEV_VERSION.$EXECUTABLE_EXTENSION

install_name_tool -change "/usr/local/lib/libftd2xx.1.2.2.dylib" "@executable_path/libftd2xx.1.2.2.dylib" "$CONFIGURATION_BUILD_DIR/$PRODUCT_NAME.$EXECUTABLE_EXTENSION";

cp -f "$CONFIGURATION_BUILD_DIR/$PRODUCT_NAME.$EXECUTABLE_EXTENSION" "$SRCROOT/../../Bin/Devices/$NewName"; 

