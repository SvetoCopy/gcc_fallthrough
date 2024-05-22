<h1>GCC Modify by Ruzal</h1>

<h2>Проблема</h2>
<p>Представим, что у нас есть какая-то non-void функция foo(int c), которая не во всех случаях возвращает значение. Например, в данном коде не возвращается значение при с != 0:</p>

```c++
#include <stdio.h>
#include <stdlib.h>

int foo(int c)
{
    if (!c)
        return 42;
    fputs("Oops\n", stderr);
}

int main(int argc, char **argv)
{
    foo(argc);
}
```

<p>Скомпилируем программу с различными флагами и посмотрим, что выводится в stderr:</p>

```
-O0 -fpic
```

![image](https://github.com/SvetoCopy/gcc_fallthrough/assets/65361271/6b294a9c-59ed-4c9f-99f6-7f02b8dbc414)


```
-O3 -fpic
```

![image](https://github.com/SvetoCopy/gcc_fallthrough/assets/65361271/c6dd8826-fb72-4aee-bba6-311e1827d458)

```
-O1 -fpic
```

![image](https://github.com/SvetoCopy/gcc_fallthrough/assets/65361271/d21cb189-c1a1-4d9a-9b01-333a20d4f8ec)

<p>Таким образом, мы видим undefined behaviour (далее UB) при различных флагах оптимизации. Это случается из-за того что программа "проваливается" из функции foo() в main(). Такая ситуация называется <b>fallthrough</b></p>

<h2>Цель</h2>
<p>Избавиться от UB и предложить однозначное решение для такого случая при всех флагах оптимизации.</p>

<h2>Пути решения проблемы</h2>
<li>Запретить компилировать программу с функцией, которая возвращает значение не при всех возможных случаях.</li>
<li>Заставить компилятор принудительно вставлять возврат значения для оставшихся случаев.</li>
<p></p>
<p>Первый способ нерационален, программисту в этом случае придется прописывать инструкции возврата, даже когда постановка задачи предполагает только один случай. Например:</p>

```c++
#include <stdio.h>
#include <stdlib.h>

int foo(int c)
{
    if (1 == 1)
        return 42;
    fputs("Oops\n", stderr);
}

int main(int argc, char **argv)
{
    foo(argc);
}
```

<p>Здесь не предполагается, что программа будет выполнять что-то кроме тела ветвления if (1 == 1). Однако компилятор должен будет запретить компиляцию такой программы.</p>
<p>Будем далее рассматривать второй способ решения проблемы.</p>

<h2>Решение</h2>
<p>Компилятор GCC имеет несколько промежуточных представлений: AST, Gimple, RTL. </p>
<p>Нам хотелось бы, чтобы модификация не зависела от флагов оптимизаций, поэтому поправку нужно делать на начальном этапе.</p>
<p>Важным уточнением является то, что компилятор GCC на самом деле распознает нашу проблему в программе и выдает предупреждение по этому поводу:</p>

![image](https://github.com/SvetoCopy/gcc_fallthrough/assets/65361271/074788b9-6dd5-4fcd-b47f-dd5e0d671d78)

<p>Давайте разберемся, как он находит это предупреждение. Для этого рассмотрим дампы, которые GCC позволяет смотреть на различных промежуточных представлениях. 
  Здесь, заметим, что на AST представлении, в конце нашей функции foo вставляется __builtin_unreachable: </p>
  
  ![image](https://github.com/SvetoCopy/gcc_fallthrough/assets/65361271/c215cb3d-7706-4357-8b6f-6c93ee7768f6)
<p>Вызов этой функции передает информацию остальным промежуточным представлениям о том, что в конце функции не обнаружен return, а это как раз наш случай.</p>
<p>Теперь у нас есть несколько путей развития:</p>
<li>Мы можем добавить инструкцию возврата из функции во время первой генерации __builtin_unreachable</li>
<li>Или же найти место, где обрабатывается __builtin_unreachable и выдается warning, там и добавить инструкцию возврата</li>

<h3>Пытаемся модифицировать генерацию __builtin_unreachable</h3>
<p>После тщательного поиска, обнаруживаем функцию, которая по описанию похожа на то, что мы ищем:</p>

```c++
/* If a function that should end with a return in non-void
   function doesn't obviously end with return, add ubsan
   instrumentation code to verify it at runtime.  If -fsanitize=return
   is not enabled, instrument __builtin_unreachable.  */

static void
cp_maybe_instrument_return (tree fndecl)
```

<p>После исследования тела функции находим место генерации нашего __builtin_unreachable</p>

```c++
location_t loc = DECL_SOURCE_LOCATION (fndecl);

if (sanitize_flags_p (SANITIZE_RETURN, fndecl))
  t = ubsan_instrument_return (loc);
else
  t = build_builtin_unreachable (BUILTINS_LOCATION);

append_to_statement_list (t, p);
```

<p>Делаем тестовую модификацию для целочисленного типа:</p>

```c++
location_t loc = DECL_SOURCE_LOCATION (fndecl);

if (sanitize_flags_p (SANITIZE_RETURN, fndecl))
  t = ubsan_instrument_return (loc);
else {
  if (INTEGRAL_TYPE_P(type)) {
    tree ret_val = build_int_cst (type, 0);
    tree decl = build_decl (loc, RESULT_DECL, NULL_TREE, type);
    tree init_expr = build2 (INIT_EXPR, type, decl, ret_val);

    t = build1 (RETURN_EXPR, void_type_node, init_expr);
  }
  else
    t = build_builtin_unreachable (BUILTINS_LOCATION);
}

append_to_statement_list (t, p);
```

<p>После компиляции видим, что вставка инструкции возврата работает корректно, однако есть один недочет: невозможно вывести warning, если мы делаем патч в этом месте.</p>
<p>Представим, что компилятор добавил инструкцию возврата для какой-то функции и не предупредил об этом. Тогда программист может <b>получать баги в своей программе и обнаружить их без предупреждения довольно тяжело.</b></p>
<p>
Такой случай, как минимум, должен быть хорошо задокументирован, однако и это не защитит в полной мере от неочевидного бага.
</p>
<p>Давайте рассмотрим другой способ решения, может быть там получится выводить warning.</p>

<h3>Пытаемся модифицировать обработку __builtin_unreachable</h3>
<p>Внутри одного из проходов замечаем обработку __builtin_unreachable на Gimple представлении:</p>

```c++
if (warning_at (location, OPT_Wreturn_type,
  "control reaches end of non-void function"))
  suppress_warning (fun->decl, OPT_Wreturn_type);
        
break;
}
```

<p>Таким образом, делаем модификацию в этом месте, оставляя вывод предупреждения:</p>

```c++
tree type = TREE_TYPE (TREE_TYPE (fun->decl));  
      
if (COMPLETE_TYPE_P (type)) {
  gsi = gsi_last;

  if (!TREE_ADDRESSABLE (type)) {
    tree tmp_decl = create_tmp_var (type, "artret");
    gimple_init_for_auto_var(tmp_decl, flag_auto_var_init, &gsi);

    greturn* ret_stmt = gimple_build_return (tmp_decl);
    gsi_insert_after (&gsi, ret_stmt, GSI_NEW_STMT);
  }
  else {
    tree ret_val = build1(INDIRECT_REF, type, DECL_RESULT(fun->decl));

    gimple_init_for_auto_var(ret_val, flag_auto_var_init, &gsi);

    greturn* ret_stmt = gimple_build_return (DECL_RESULT(fun->decl));
    gsi_insert_after (&gsi, ret_stmt, GSI_NEW_STMT);
  }

  edge e = make_edge (bb, EXIT_BLOCK_PTR_FOR_FN(fun), 0);
  e->goto_locus = location;
}

if (warning_at (location, OPT_Wreturn_type,
  "control reaches end of non-void function"))
  suppress_warning (fun->decl, OPT_Wreturn_type);
        
break;
}
```
<p>Здесь обработан случай с возвращемым значением в виде структуры (необходимо было отдельно выделить этот случай из-за особенностей архитектуры gcc). А также добавлена возможность задавать стандартные значения через flag_auto_var_init
</p>

<h2>Заключение</h2>
<p>В итоге, модификация прошла все стандартные тесты GCC, то есть патч не повлиял на работу компилятора, кроме нашего случая. Также были пройдены небольшие тесты на работоспособность патча при различных возвращаемых типах.
  Можно сказать, что решение проблемы найдено.
</p>






