/**
 * @file list.c
 * @author qr-kou (codinlog@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2021-10-26
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "list.h"

const char LIST[] = "list\0";

static AnyPtr list_at(const ListPtr list_ptr, const size_t poition)
{
    if (poition >= list_ptr->_size)
    {
        return RT_NULL;
    }
    return list_ptr->_item[poition];
}

static AnyPtr list_next(const ListPtr list_ptr)
{

    return (list_ptr->_item[list_ptr->_cursor++]);
}

static rt_bool_t list_has_next(const ListPtr list_ptr)
{
    return list_ptr->_size > list_ptr->_cursor;
}

static void list_foreach(const ListPtr list_ptr, void (*callback)(const AnyPtr item))
{
    for (size_t i = 0; i < list_ptr->_size; i++)
    {
        const AnyPtr item = list_ptr->_item[i];
        callback(item);
    }
}

inline static MusicCPtr list_music_parse_from_cjson_to_cptr(const cJSON *cjson)
{
    MusicPtr music_ptr = MusicManager.from_cjson_to_cptr(cjson);
    return music_ptr;
}

static List list_music_from_cjson(const cJSON *cjson)
{
    List list = {
        ._size = 0,
        ._cursor = 0,
        ._item = RT_NULL,
    };
    if (!cJSON_IsArray(cjson))
    {
        return list;
    }

    size_t size = cJSON_GetArraySize(cjson);
    MusicPtr *music_ptr = (MusicPtr *)rt_malloc(size * sizeof(MusicPtr));
    for (size_t i = 0; i < size; i++)
    {
        const cJSON *music_cjson = cJSON_GetArrayItem(cjson, i);
        music_ptr[i] = list_music_parse_from_cjson_to_cptr(music_cjson);
    }
    list._size = size;
    list._item = music_ptr;
    return list;
}

static ListCPtr list_music_from_cjson_to_cptr(const cJSON *cjson)
{
    if (cjson == RT_NULL)
    {
        return RT_NULL;
    }

    if (cJSON_IsNull(cjson))
    {
        return RT_NULL;
    }

    if (!cJSON_IsArray(cjson))
    {
        return RT_NULL;
    }
    const size_t size = cJSON_GetArraySize(cjson);
    MusicPtr *music_ptr = (MusicPtr *)rt_malloc(size * sizeof(MusicPtr));
    for (size_t i = 0; i < size; i++)
    {
        const cJSON *music_cjson = cJSON_GetArrayItem(cjson, i);
        music_ptr[i] = list_music_parse_from_cjson_to_cptr(music_cjson);
    }

    ListPtr list_ptr = (ListPtr)rt_malloc(sizeof(List));
    list_ptr->_size = size;
    list_ptr->_cursor = 0;
    list_ptr->_item = music_ptr;
    return list_ptr;
}

static void list_music_drop_memory_entry(const AnyPtr item)
{
    const MusicPtr music_ptr = (MusicPtr)item;
    MusicManager.drop_memory_and_self(music_ptr);
}

static void list_music_drop_memory(const ListPtr list_ptr)
{
    if (list_ptr != RT_NULL)
    {
        list_foreach(list_ptr, list_music_drop_memory_entry);
    }
}

static List
list_from_cjson(const cJSON *cjson, ListTypeEnum type)
{
    if (type == MUSIC)
    {
        return list_music_from_cjson(cjson);
    }
}

static ListCPtr list_from_cjson_to_cptr(const cJSON *cjson, ListTypeEnum type)
{
    if (cjson == RT_NULL)
    {
        return RT_NULL;
    }

    if (cJSON_IsNull(cjson))
    {
        return RT_NULL;
    }

    if (!cJSON_IsArray(cjson))
    {
        return RT_NULL;
    }

    printf("cjson is not null \0");
    if (type == MUSIC)
    {
        return list_music_from_cjson_to_cptr(cjson);
    }
}

static void list_drop_memory(const ListPtr list_ptr, ListTypeEnum type)
{
    if (list_ptr != RT_NULL)
    {
        if (type == MUSIC)
        {
            list_music_drop_memory(list_ptr);
        }
        rt_free(list_ptr->_item);
    }
}

static void list_drop_memory_and_self(const ListPtr list_ptr, ListTypeEnum type)
{
    list_drop_memory(list_ptr, type);
    rt_free(list_ptr);
}

const _ListManager ListManager = {
    .from_cjson = list_from_cjson,
    .from_cjson_to_cptr = list_from_cjson_to_cptr,
    .has_next = list_has_next,
    .next = list_next,
    .at = list_at,
    .foreach = list_foreach,
    .drop_memory = list_drop_memory,
    .drop_memory_and_self = list_drop_memory_and_self,
};
