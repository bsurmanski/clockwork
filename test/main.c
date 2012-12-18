/**
 * main.c
 * November 18, 2012
 * Brandon Surmanski
 */

#include <GL/glew.h>
#include <GL/gl.h>
#include <alloca.h>
#include <assert.h>
#include <errno.h>
#include <float.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "clockwork/util/math/stats.h"
#include "clockwork/util/hash.h"
#include "clockwork/util/math/vec.h"
#include "clockwork/util/math/scalar.h"
#include "clockwork/util/math/tri.h"
#include "clockwork/util/math/matrix.h"
#include "clockwork/util/math/convert.h"
#include "clockwork/util/struct/iterator.h"
#include "clockwork/util/struct/kdtree.h"
#include "clockwork/util/struct/list.h"
#include "clockwork/util/str.h"

#define SECTION_BEGIN(msg) printf("***Testing %s***\n",msg)
#define SECTION_END(msg) printf("***Passed %s***\n\n",msg)
#define TEST(msg,val) if(val && msg){printf("Passed %s\n",msg);}else if(!val){printf("!!!Failed %s!!!\n",msg);}
#define TEST_BEGIN(msg) printf("Testing %s\n",msg)
#define TEST_END(msg) printf("Passed %s\n",msg)

#ifndef DEBUG
#define NDEBUG
#endif

void test_glExt(void);
void test_vec(void);
void test_str(void);
void test_matrix(void);
void test_mesh(void);
bool unit_test(bool ignore);

//OpenGL ability
void test_glExt(void)
{
    assert(GL_VERSION_3_0 && "GL Version < 3.0");
    assert(GL_ARB_vertex_array_object && "Vertex Array capabilities not found. \
                Please update Graphics drivers");
    assert(GL_ARB_instanced_arrays && "Instanced array capabilities not found. \
                Please update Graphics drivers");
    assert(GL_EXT_draw_instanced && "Cannot draw instances meshes.");
    assert(GL_BGRA && "BGRA Color representation not supported");
}

int test_stats(void)
{
    SECTION_BEGIN("Stats");
    assert(1140 == stats_choose(20, 3));
    assert(35 == stats_choose(7, 3));
    assert(0 == stats_choose(2, 8));
    SECTION_END("Stats");
}

void test_str(void)
{
    SECTION_BEGIN("str");
    str s;
    str_init(&s, "Some long text or something");
    TEST_BEGIN("find string");
    assert(str_find_cstr(&s, "or") == 15);
    str_reset(&s, "some other string 1232 and stuffy");
    assert(str_find_cstr(&s, " 12") == 17);
    assert(str_find_cstr(&s, "stringy") == -1);
    TEST_END("find string");
    SECTION_END("str");
}

//vec.h
void test_vec(void)
{
    SECTION_BEGIN("Vector Math");
    //htof
    TEST_BEGIN("Half-To-Float");
    assert(feq(htof(32767), 1.0f));
    assert(feq(htof(-32768), -1.0f));
    assert(feq(htof(0), 0.0f));
    TEST_END("Half-To-Float");
    //uhtof
    TEST_BEGIN("Unsigned-Half-To-Float");
    assert(feq(uhtof(0), 0.0f));
    assert(feq(uhtof(65535), 1.0f));
    TEST_END("Unsigned-Half-To-Float");
    //vector average
    TEST_BEGIN("Averaging");
    vec3 a = {1,2,3};
    vec3 b = {2,3,4};
    vec3 c = {1.4f, 1.4f, 1.4f};
    vec3 d = {0,0,0};
    vec3 res;
    vec3 tot = {1.1f, 1.6f, 2.1f};
    vec3_avg(res, 4, a, b, c, d);
    assert(vec3_eq(res, tot));
    TEST_END("Averaging");
    SECTION_END("Vector Math\n");
}

void test_matrix(void)
{
    SECTION_BEGIN("Matrix Math"); 
    TEST_BEGIN("Quaternion Conversion");
    mat4 m;
    quaternion q;
    mat4_identity(m);
    mat4_to_quaternion(m, q);
    vec4_print(q);
    //TODO: assert q = (0,0,0,1);
    TEST_END("Quaternion Conversion");
    SECTION_END("Matrix Math");
}

void test_list(void)
{
    SECTION_BEGIN("List");
    List list; 
    printf("list init\n");
    list_init(&list, sizeof(int));
    assert(list_length(&list) == 0);
    printf("list adding\n");
    int n = 1;
    list_addfront(&list, &n);
    n = 2; list_addfront(&list, &n);
    n = 3; list_addfront(&list, &n);
    n = 10; list_addback(&list, &n);
    assert(list_length(&list) == 4);
    Iterator front_it;
    Iterator back_it;
    list_first(&list, &front_it);
    list_next(&list, &front_it);
    list_last(&list, &back_it);
    list_moveafter(&list, &front_it, &back_it);
    list_remove(&list, &front_it);
    Iterator it;
    list_first(&list, &it);
    printf("list iterating\n");
    while(iterator_value(&it))
    {
        printf("val: %d\n", *((int*)iterator_value(&it)));
        list_next(&list, &it);
    }
    assert(list_datasize(&list) == sizeof(int) * 3);
    int *buf = alloca(list_datasize(&list));
    list_tobuffer(&list, buf);
    
    int i;
    for(i = 0; i < 3; i++)
    {
        printf("bufn: %d\n", buf[i]);
    }
    SECTION_END("List");
}

int main(int argc, char **argv)
{
    test_str();
    test_vec();
    test_matrix();
    test_stats(); 
    test_list();
}
