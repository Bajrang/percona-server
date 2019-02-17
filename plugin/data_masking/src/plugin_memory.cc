/* Copyright (c) 2018, 2019 Francisco Miguel Biete Banon. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#include "plugin/data_masking/include/plugin_memory.h"

void init_data_masking_psi_keys()
{
  PSI_mutex_info all_data_masking_mutex[] = {
    {&key_data_masking_mutex, "data_masking::mutex", PSI_FLAG_SINGLETON, 0, PSI_DOCUMENT_ME}
  };

  PSI_memory_info all_data_masking_memory[] = {
    {&key_memory_data_masking, "data_masking", 0, 0, PSI_DOCUMENT_ME}
  };

  const char *category = "data_masking";
  int count;

  count = static_cast<int>(array_elements(all_data_masking_memory));
  mysql_memory_register(category, all_data_masking_memory, count);

  count = static_cast<int>(array_elements(all_data_masking_mutex));
  mysql_mutex_register(category, all_data_masking_mutex, count);
}

void init_data_masking_memory()
{
  #ifdef HAVE_PSI_INTERFACE
    init_data_masking_psi_keys();

    // Allocate dynamic variables at plugin level
    void *pBuffer = (t_mask_dict *)my_malloc(key_memory_data_masking, sizeof(t_mask_dict), MYF(0));
  #else
    void *pBuffer = (t_mask_dict *)my_malloc(sizeof(t_mask_dict), MYF(0));
  #endif // HAVE_PSI_INTERFACE
    if (pBuffer)
      g_data_masking_dict = new(pBuffer) t_mask_dict();

    mysql_mutex_init(key_data_masking_mutex, &g_data_masking_dict_mutex, MY_MUTEX_INIT_FAST);
}

void deinit_data_masking_memory()
{
  // Destroy global variables and mutex protecting them
  mysql_mutex_lock(&g_data_masking_dict_mutex);
  g_data_masking_dict->~t_mask_dict();
  my_free(g_data_masking_dict);
  mysql_mutex_unlock(&g_data_masking_dict_mutex);

  mysql_mutex_destroy(&g_data_masking_dict_mutex);
}
