/*
 * Copyright 2009-2010 10gen, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file contains C implementations of some of the functions needed by the
 * bson module. If possible, these implementations should be used to speed up
 * BSON encoding and decoding.
 */

#include "ruby.h"

#if HAVE_RUBY_ST_H
#include "ruby/st.h"
#endif
#if HAVE_ST_H
#include "st.h"
#endif

#if HAVE_RUBY_REGEX_H
#include "ruby/regex.h"
#endif
#if HAVE_REGEX_H
#include "regex.h"
#endif

#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>


static VALUE ObjectId;
static VALUE InvalidObjectId;

static int legal_objectid_str(VALUE str) {
    int i;

    if (TYPE(str) != T_STRING) {
        return 0;
    }

    if (RSTRING_LEN(str) != 24) {
        return 0;
    }

    for(i = 0; i < 24; i++) {
        char c = RSTRING_PTR(str)[i];

        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            return 0;
        }
    }

    return 1;
}

static VALUE objectid_legal(VALUE self, VALUE str)
{
    if (legal_objectid_str(str))
        return Qtrue;
    return Qfalse;
}

static char hexbyte( char hex ) {
    if (hex >= '0' && hex <= '9')
        return (hex - '0');
    else if (hex >= 'A' && hex <= 'F')
        return (hex - 'A' + 10);
    else if (hex >= 'a' && hex <= 'f')
        return (hex - 'a' + 10);
    else
        return 0x0;
}

static VALUE objectid_from_string(VALUE self, VALUE str)
{
    VALUE oid, object_id;
    int i;

    if (!legal_objectid_str(str)) {
        rb_raise(InvalidObjectId, "illegal ObjectId format: %s", RSTRING_PTR(str));
    }

    oid = rb_ary_new2(12);

    for(i = 0; i < 12; i++) {
        rb_ary_store(oid, i, INT2FIX( (unsigned)(hexbyte( RSTRING_PTR(str)[2*i] ) << 4 ) | hexbyte( RSTRING_PTR(str)[2*i + 1] )));
    }

    object_id = rb_class_new_instance(1, &oid, ObjectId);
    rb_iv_set(object_id, "@cached_string", str);

    return object_id;
}

// def to_s
//   @data.map {|e| v=e.to_s(16); v.size == 1 ? "0#{v}" : v }.join
// end
static VALUE objectid_to_s(VALUE self)
{
    VALUE data;
    VALUE cached_string;
    char cstr[25];
    VALUE rstr;
    VALUE *data_arr;

    cached_string = rb_iv_get(self, "@cached_string");

    if ( cached_string != Qnil )
        return cached_string;

    data = rb_iv_get(self, "@data");
    data_arr = RARRAY_PTR(data);

    sprintf(cstr, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        NUM2INT(data_arr[0]), NUM2INT(data_arr[1]), NUM2INT(data_arr[2]), NUM2INT(data_arr[3]),
        NUM2INT(data_arr[4]), NUM2INT(data_arr[5]), NUM2INT(data_arr[6]), NUM2INT(data_arr[7]),
        NUM2INT(data_arr[8]), NUM2INT(data_arr[9]), NUM2INT(data_arr[10]), NUM2INT(data_arr[11]));

    rstr = rb_str_new(cstr, 24);
    rb_iv_set(self, "@cached_string", rstr);

    return rstr;
}

void Init_bson_object_id() {
    VALUE bson;

    rb_require("bson");
    bson = rb_const_get(rb_cObject, rb_intern("BSON"));
    rb_require("bson/types/object_id");
    ObjectId = rb_const_get(bson, rb_intern("ObjectId"));
    rb_require("bson/exceptions");
    InvalidObjectId = rb_const_get(bson, rb_intern("InvalidObjectId"));

    rb_define_singleton_method(ObjectId, "legal?", objectid_legal, 1);
    rb_define_singleton_method(ObjectId, "from_string", objectid_from_string, 1);

    rb_define_method(ObjectId, "to_s", objectid_to_s, 0);
}
