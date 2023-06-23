# Mandatory line specifying the minimum CMAKE version
cmake_minimum_required(VERSION 3.16.0)

# Mandatory line, includes necessary cmake functionality   
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Optional - Adds additional component directory that will be searched for included files (Defaults are $ESP_IDF/components and $Project/components)
list(APPEND EXTRA_COMPONENT_DIRS /home/iot/esp/esp-idf-lib/components)

# Mandatory, creates project itself and is used when naming the generated binary. 
project(tst-dht-01-fu)
