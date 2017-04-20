/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GSTD_PROPERTY_BOOLEAN_H__
#define __GSTD_PROPERTY_BOOLEAN_H__

#include <glib-object.h>

#include "gstd_property.h"

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define GSTD_TYPE_PROPERTY_BOOLEAN \
  (gstd_property_boolean_get_type())
#define GSTD_PROPERTY_BOOLEAN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GSTD_TYPE_PROPERTY_BOOLEAN,GstdPropertyBoolean))
#define GSTD_PROPERTY_BOOLEAN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GSTD_TYPE_PROPERTY_BOOLEAN,GstdPropertyBooleanClass))
#define GSTD_IS_PROPERTY_BOOLEAN(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSTD_TYPE_PROPERTY_BOOLEAN))
#define GSTD_IS_PROPERTY_BOOLEAN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GSTD_TYPE_PROPERTY_BOOLEAN))
#define GSTD_PROPERTY_BOOLEAN_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GSTD_TYPE_PROPERTY_BOOLEAN, GstdPropertyBooleanClass))

typedef struct _GstdPropertyBoolean GstdPropertyBoolean;
typedef struct _GstdPropertyBooleanClass GstdPropertyBooleanClass;
GType gstd_property_boolean_get_type();


struct _GstdPropertyBoolean
{
  GstdProperty parent;
};

struct _GstdPropertyBooleanClass
{
    GstdPropertyClass parent_class;
};

GstdReturnCode gstd_property_boolean_to_string (GstdObject * self, gchar ** outstring);

G_END_DECLS

#endif // __GSTD_PROPERTY_BOOLEAN_H__