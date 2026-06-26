# Vendored Unity

[Unity](https://github.com/ThrowTheSwitch/Unity) test framework (MIT licensed, see
`LICENSE.txt`), used by the host-side logic unit tests in `firmware/test/`.

These sources (`unity.c`, `unity.h`, `unity_internals.h`) were copied from the copy that
ships with ESP-IDF (`$IDF_PATH/components/unity/unity/src`). Vendoring keeps the host tests
buildable with just CMake + a host C compiler, independent of the ESP-IDF "linux" target
(which does not build on native Windows).
