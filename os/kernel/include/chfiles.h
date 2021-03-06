/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    chfiles.h
 * @brief   Data files.
 * @details This header defines abstract interfaces useful to access generic
 *          data files in a standardized way.
 *
 * @addtogroup data_files
 * @details This module define an abstract interface for generic data files by
 *          extending the @p BaseSequentialStream interface. Note that no code
 *          is present, data files are just abstract interface-like structures,
 *          you should look at the systems as to a set of abstract C++ classes
 *          (even if written in C). This system has the advantage to make the
 *          access to streams independent from the implementation logic.<br>
 *          The data files interface can be used as base class for high level
 *          object types such as an API for a File System implementation.
 * @{
 */

#ifndef _CHFILES_H_
#define _CHFILES_H_

/**
 * @brief   Error code from the file stream methods.
 */
#define FILE_ERROR 0xFFFFFFFFUL

/**
 * @brief   File offset type.
 */
typedef uint32_t fileoffset_t;

/**
 * @brief   BaseFileStream specific methods.
 */
#define _base_file_stream_methods                                           \
  _base_sequential_stream_methods                                           \
  /* File close method.*/                                                   \
  uint32_t (*close)(void *instance);                                        \
  /* Get last error code method.*/                                          \
  int (*geterror)(void *instance);                                          \
  /* File get size method.*/                                                \
  fileoffset_t (*getsize)(void *instance);                                  \
  /* File get current position method.*/                                    \
  fileoffset_t (*getposition)(void *instance);                              \
  /* File seek method.*/                                                    \
  fileoffset_t (*lseek)(void *instance, fileoffset_t offset);

/**
 * @brief   @p BaseFileStream specific data.
 * @note    It is empty because @p BaseFileStream is only an interface
 *          without implementation.
 */
#define _base_file_stream_data                                              \
  _base_sequential_stream_data

/**
 * @extends BaseSequentialStreamVMT
 *
 * @brief   @p BaseFileStream virtual methods table.
 */
struct BaseFilelStreamVMT {
  _base_file_stream_methods
};

/**
 * @extends BaseSequentialStream
 *
 * @brief   Base file stream class.
 * @details This class represents a generic file data stream.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct FileStreamVMT *vmt;
  _base_file_stream_data
} BaseFileStream;

/**
 * @name    Macro Functions (BaseFileStream)
 * @{
 */
/**
 * @brief   Base file Stream close.
 * @details The function closes a file stream.
 *
 * @param[in] ip        pointer to a @p BaseFileStream or derived class
 *
 * @api
 */
#define chFileStreamClose(ip) ((ip)->vmt->close(ip))

/**
 * @brief   Returns an implementation dependent error code.
 *
 * @param[in] ip        pointer to a @p BaseFileStream or derived class
 *
 * @api
 */
#define chFileStreamGetError ((ip)->vmt->geterror(ip))

/**
 * @brief   Returns the current file size.
 *
 * @param[in] ip        pointer to a @p BaseFileStream or derived class
 *
 * @api
 */
#define chFileStreamGetSize ((ip)->vmt->getposition(ip))

/**
 * @brief   Returns the current file pointer position.
 *
 * @param[in] ip        pointer to a @p BaseFileStream or derived class
 *
 * @api
 */
#define chFileStreamGetPosition ((ip)->vmt->getposition(ip))

/**
 * @brief   Moves the file current pointer to an absolute position.
 *
 * @param[in] ip        pointer to a @p BaseFileStream or derived class
 * @param[in] offset    new absolute position
 *
 * @api
 */
#define chFileStreamSeek ((ip)->vmt->lseek(ip, offset))
/** @} */

#endif /* _CHFILES_H_ */

/** @} */
