#!/bin/sh
echo ---------------------------------
echo ------ post build script --------
echo ---------------------------------

NewName=$PRODUCT_NAME"_"$MODULE_USER_VERSION"_"$MODULE_DEV_VERSION.$EXECUTABLE_EXTENSION

cp -f "$CONFIGURATION_BUILD_DIR/$PRODUCT_NAME.$EXECUTABLE_EXTENSION" "$SRCROOT/../../Bin/Devices/$NewName"
