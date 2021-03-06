#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "mpconfigboard.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "w25qxx.h"


STATIC mp_obj_t py_gc_heap_size(size_t n_args, const mp_obj_t *args) {
    config_data_t config;
    load_config_from_spiffs(&config);
    if(n_args == 0)
        return mp_obj_new_int(config.gc_heap_size);
    else if(n_args != 1)
        mp_raise_OSError(MP_EINVAL);
    config.gc_heap_size = mp_obj_get_int(args[0]);
    if( !save_config_to_spiffs(&config) )
        mp_raise_OSError(MP_EIO);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_gc_heap_size_obj, 0, 1, py_gc_heap_size);

// STATIC mp_obj_t py_malloc(mp_obj_t arg) {
//     void malloc_stats(void);
//     malloc_stats();
//     void* p = malloc(mp_obj_get_int(arg));
//     return mp_obj_new_int((mp_int_t)p);
// }

// STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_malloc_obj, py_malloc);

// STATIC mp_obj_t py_free(mp_obj_t arg) {
//     free(mp_obj_get_int(arg));
//     return mp_const_none;
// }

// STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_free_obj, py_free);


STATIC mp_obj_t py_flash_read(mp_obj_t addr, mp_obj_t len_in) {
    size_t length = mp_obj_get_int(len_in);
    byte* data = m_new(byte, length);
    w25qxx_status_t status = w25qxx_read_data_dma(mp_obj_get_int(addr), data, (uint32_t)length, W25QXX_QUAD_FAST);
    if(status != W25QXX_OK)
    {
        mp_raise_OSError(MP_EIO);
    }
    return mp_obj_new_bytes(data, length);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_flash_read_obj, py_flash_read);

static const mp_map_elem_t locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_utils) },
    { MP_ROM_QSTR(MP_QSTR_gc_heap_size),    (mp_obj_t)(&py_gc_heap_size_obj) },
    // { MP_ROM_QSTR(MP_QSTR_malloc),    (mp_obj_t)(&py_malloc_obj) },
    // { MP_ROM_QSTR(MP_QSTR_free),    (mp_obj_t)(&py_free_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_read),    (mp_obj_t)(&py_flash_read_obj) },
};
STATIC MP_DEFINE_CONST_DICT(locals_dict, locals_dict_table);

const mp_obj_type_t Maix_utils_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_utils,
    .locals_dict = (mp_obj_dict_t*)&locals_dict
};