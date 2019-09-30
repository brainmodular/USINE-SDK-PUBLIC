#!/bin/sh
echo ---------------------------------
echo ------ post build script --------
echo ---------------------------------

NewName=$PRODUCT_NAME.$EXECUTABLE_EXTENSION

cp -f "$CONFIGURATION_BUILD_DIR/$PRODUCT_NAME.$EXECUTABLE_EXTENSION" "$SRCROOT/../../Bin/Devices/$NewName"
