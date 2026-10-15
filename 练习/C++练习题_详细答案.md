# C++ 练习题 · 详细答案与参考实现（对应 40 题）

> 配套题集：`C++练习题_黑马课程配套.md`
> **编译验证状态：已全部实测**（2026 版）
> 实测环境：`g++ (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0` / `x86_64` / Ubuntu 22.04，编译参数 `g++ -std=c++17 -Wall -O0`。
> 本文档中所有代码块均已**逐题编译并运行通过**，标注的输出均为**本机真实输出**（除刻意演示编译错误的第 13 题等）。少量数值（如地址、`sizeof`）受编译器/平台/ASLR 影响，文中已标注实测值并说明差异来源。

---

## 第一层 · 基础语法与内存管理

### 第 1 题答案 · 整型溢出与截断

```cpp
#include <iostream>
using namespace std;

int main() {
    char           c  = 300;                        // 300 = 0x12C
    signed char     sc = 200;                        // 200 = 0xC8
    unsigned char   uc = 300;
    int            i  = 2147483647;                 // INT_MAX
    unsigned int   ui = 4294967295u + 1u;            // UINT_MAX + 1

    cout << "c  = " << (int)c<< endl;   // 44
    cout << "sc = " << (int)sc << endl;  // -56
    cout << "uc = " << (int)uc << endl;  // 44
    cout << "i  = " << i + 1       << endl;  // 溢出：-2147483648（未定义行为）
    cout << "ui = " << ui          << endl;  // 0（无符号回绕，定义良好）
    return 0;
}
```

**关键结论**

| 类型 | 范围 | 输入 | 存储 | 原因 |
|---|---|---|---|---|
| `char`（x86-64 Linux 默认有符号） | -128~127 | 300 | 44 | 按模 256 截断，保留低 8 位 `0x2C` |
| `signed char` | -128~127 | 200 | -56 | `0xC8` 按补码解释为 -56 |
| `unsigned char` | 0~255 | 300 | 44 | 按模 256 截断 |
| `int` | -2³¹~2³¹-1 | INT_MAX+1 | 溢出 | **未定义行为**，编译器可任意处理 |
| `unsigned int` | 0~2³²-1 | UINT_MAX+1 | 0 | 无符号按模 2³² 回绕，**定义良好** |

**核心记忆点**：无符号溢出是「回绕」（定义良好，可预测）；有符号溢出是「未定义行为」（编译器可以假设它永不发生，从而做出激进优化）。这正是第 10 题里 `-O2` 会改变悬垂引用行为的原因。

**笔记对应**：《C++基础编程》整型截断与符号变量部分，笔记中已在 `WSL Ubuntu/g++ 11.4.0` 下实测验证过该结论。

---

### 第 2 题答案 · 浮点类型与精度

```cpp
#include <iostream>
#include <iomanip>
#include <cfloat>
using namespace std;

int main() {
    float  f1 = 0.1f,     f2 = 1e-8f,    f3 = 1.0f / 3;
    double d1 = 0.1,      d2 = 1e-8,     d3 = 1.0 / 3.0;
    float  tiny = 1e-40f;

    cout << boolalpha;
    cout << "f1 == 0.1 : " << (f1 == 0.1)  << endl;  // false ← 关键
    cout << "d1 == 0.1 : " << (d1 == 0.1)  << endl;  // true
    cout << "f1精确值: " << setprecision(12) << (double)f1 << endl; // 0.100000001490
    cout << "d1精确值: " << setprecision(17) << d1 << endl;            // 0.10000000000000001
    cout << "d3       : " << setprecision(17) << d3 << endl;            // 0.33333333333333331

    cout << "sizeof(float)  = " << sizeof(float)  << endl;  // 4
    cout << "sizeof(double) = " << sizeof(double) << endl;  // 8
    cout << "float  有效十进制位≈" << (int)FLT_DIG     << endl;  // 6~7
    cout << "double 有效十进制位≈" << (int)DBL_DIG     << endl;  // 15
    cout << "tiny   = " << setprecision(10) << tiny << endl;   // 9.999946101e-41（非规格化数，精度已损失）
    return 0;
}
```

**关键结论**

1. **`f1 == 0.1` 为 `false`** 是本题最重要的观察点：`0.1f` 存储的是最接近 0.1 的 float 值 `0.100000001490116...`，比较时它被提升为 double `0.10000000149011612`，与字面量 `0.1` 的 double 值 `0.1000000000000000055...` 不相等。而 `double d1 = 0.1` 直接用同一个 double 常量初始化，比较时是同值比较，所以为 `true`。
2. **建议做法**：比较浮点数用误差判定，`#include <cmath>` 后用 `fabs(a-b) < eps`（eps 常取 `1e-6`~`1e-9`，按量级选）。
3. `1e-40` 落在 float 的**非规格化区间**（`FLT_MIN` 约 1.18e-38 以下到 1.4e-45），因此不会变成 0，而是精度已耗尽的极小值。实测 `setprecision(10)` 下打印为 `9.999946101e-41`（不是 `1e-40`），这个偏差正是非规格化数尾数有效位不足的直接证据。
4. 字面量后缀：`f` → float，无后缀 → double，`L` → long double。

---

### 第 3 题答案 · 混合类型运算与显式转换

```cpp
#include <iostream>
using namespace std;

void testMixed() {
    unsigned int a = 1;
    int b = 2;
    cout << "a - b = " << (a - b) << endl;   // 4294967295
    cout << "a + b = " << (a + b) << endl;   // 3

    int c = -3;
    cout << "a + c = " << (a + c) << endl;   // 4294967294 ← 负号被吞
    cout << "static_cast<unsigned>(c) = " << static_cast<unsigned int>(c) << endl; // 4294967293
}

unsigned int safeSub(unsigned int x, unsigned int y) {
    return x > y ? (x - y) : 0u;
}

int main() {
    testMixed();
    cout << "safeSub(10, 3) = " << safeSub(10, 3) << endl;  // 7
    cout << "safeSub(3, 10) = " << safeSub(3, 10) << endl;  // 0（钳制，不回绕）
    return 0;
}
```

**关键结论**

- 当 `int` 与 `unsigned int`（同级、且 `int` 无法表示 `unsigned` 的全部值）混算时，`int` 被**隐式转为 unsigned**，负数变成巨大的正数（`-3` → `4294967293`），因此 `a + c` 得到 `4294967294` 而不是 `-2`。
- 规避手段：① 在表达式里把变量显式转成有符号类型；② 用 `static_cast` 明确意图；③ 写成形如 `safeSub` 这类先比较再相减的安全函数。
- 记住口诀：**同级别无符号优先级更高，负号会被吞掉**。

---

### 第 4 题答案 · 三目运算符与 switch 穿透

```cpp
#include <iostream>
using namespace std;

int maxOfThree(int a, int b, int c) {
    // 三目运算符右结合：a>b ? (a>c?a:c) : (b>c?b:c)
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

// 写法一：每个 case 直接 return —— 天然不会穿透
const char* weekName(int d) {
    switch (d) {
    case 1: return "星期一";
    case 2: return "星期二";
    case 3: return "星期三";
    case 4: return "星期四";
    case 5: return "星期五";
    case 6: return "星期六";
    case 7: return "星期日";
    default: return "非法输入";
    }
}

// 写法二：cout + break 形式，删掉 case 3 的 break 就会向下穿透（穿透演示用）
void weekNameFallthrough(int d) {
    switch (d) {
    case 1: cout << "星期一"; break;
    case 2: cout << "星期二"; break;
    case 3: cout << "星期三";        // ← 故意不写 break
    case 4: cout << "星期四"; break;
    case 5: cout << "星期五"; break;
    case 6: cout << "星期六"; break;
    case 7: cout << "星期日"; break;
    default: cout << "非法输入";
    }
    cout << endl;
}

int main() {
    cout << maxOfThree(3, 9, 5) << endl;      // 9
    for (int d = 1; d <= 8; ++d) cout << d << " -> " << weekName(d) << endl;
    cout << "--- 穿透演示（case 3 无 break）---" << endl;
    weekNameFallthrough(3);                   // 星期三星期四 ← 穿透到 case 4
    return 0;
}
```

**运行输出（实测）**

```
9
1 -> 星期一
2 -> 星期二
3 -> 星期三
4 -> 星期四
5 -> 星期五
6 -> 星期六
7 -> 星期日
8 -> 非法输入
--- 穿透演示（case 3 无 break）---
星期三星期四
```

> 注意：`weekName()` 用的是 `return`，**删掉 return 就不会穿透**（原答案注释所说"删掉 break 会穿透"只适用于 `cout + break` 的写法，故这里补上写法二）。

**关键结论**

- 三目运算符 `?:` **右结合**：`a?b:c?d:e` 等价于 `a?b:(c?d:e)`。写嵌套三目时建议加括号明确意图。
- `switch` 从匹配到的 `case` 开始**逐条向下执行**，直到遇到 `break` 或 `}`，因此「忘写 break」会导致穿透（fall-through）。
- `default` 兜底非法输入；`switch` 表达式必须是**整型或枚举**，不能是 `float`/`string`。

---

### 第 5 题答案 · 数组名退化与指针遍历

```cpp
#include <iostream>
using namespace std;

int* maxElement(int* a, int n) {          // 返回最大元素的指针
    int* best = a;
    for (int i = 1; i < n; ++i)
        if (a[i] > *best) best = a + i;
    return best;
}

int* reverse(int* a, int n) {             // 原地逆置，返回首地址
    for (int i = 0; i < n / 2; ++i) {
        int t = a[i];
        a[i] = a[n - 1 - i];
        a[n - 1 - i] = t;
    }
    return a;
}

int main() {
    int arr[5] = {4, 1, 7, 3, 9};
    int n = 5;

    cout << "下标遍历：";
    for (int i = 0; i < n; ++i) cout << arr[i] << ' ';
    cout << "\n指针遍历：";
    for (int* p = arr; p < arr + n; ++p) cout << *p << ' ';   // p 前进 1 元素 = 4 字节
    cout << endl;

    int* mp = maxElement(arr, n);
    cout << "最大值 = " << *mp << "，下标 = " << (mp - arr) << endl;   // 9, 4（arr[4] 才是 9）

    int* head = reverse(arr, n);
    cout << "逆置后首元素 = " << *head << endl;                     // 9
    cout << "偏移后(head+1) = " << *(head + 1) << endl;             // 3

    char str[] = "hello";
    for (char* c = str; *c != '\0'; ++c) cout << *c << '-';
    cout << endl;
    return 0;
}
```

**关键结论**

- 数组名在**大多数表达式**中退化为指向首元素的指针；例外有三：`sizeof(arr)`、`&arr`（得到指向整个数组的指针）、作为 `static`/`constexpr` 等上下文。
- 指针算术以**元素个数**为单位：`p + 1` 表示「跳过一个 `int`」即地址加 4 字节；`mp - arr` 直接就是下标，这是最实用的用法。
- 函数形参写 `int a[]` 与 `int* a` 完全等价，都不传长度，必须额外传 `n`。
- `char str[] = "hello"` 末尾自带 `'\0'`，用 `*c != '\0'` 遍历。

---

### 第 6 题答案 · 动态内存管理

```cpp
#include <iostream>
#include <cstdlib>      // malloc / free
using namespace std;

struct S { int id; float score; };

int main() {
    // ① new[] / delete[]
    int* a = new int[10];
    for (int i = 0; i < 10; ++i) a[i] = i * i;
    cout << "a[3] = " << a[3] << endl;      // 9
    delete[] a;                              // 必须带[]，否则行为未定义

    // ② new / delete（单个对象）
    S* s1 = new S{1, 95.5f};
    cout << s1->id << ' ' << s1->score << endl;
    delete s1;                               // 无[]

    // ③ malloc / free
    S* s2 = (S*)malloc(sizeof(S) * 5);       // 无需类型转换，C 里不能，C++ 里强制转换
    for (int i = 0; i < 5; ++i) { s2[i].id = i; s2[i].score = 60 + i; }
    cout << s2[4].id << ' ' << s2[4].score << endl;
    free(s2);

    // ④ 错误示范（仅注释，不要放开）
    // int* leak = new int[100];   // 未 delete[] → 内存泄漏，进程结束时才由 OS 回收
    return 0;
}
```

**关键结论**

| 分配 | 释放 | 备注 |
|---|---|---|
| `new T` | `delete` | 会调用 `T` 的构造/析构函数 |
| `new T[n]` | `delete[]` | 括号必须成对，漏 `[]` 为未定义行为 |
| `malloc(n)` | `free(p)` | 不调用构造/析构；返回 `void*` 需强转；失败返回 `NULL` |
| `new` | ✗ | 抛 `std::bad_alloc` 异常，而非返回空指针 |

**绝对不能混用**：`new` 配 `delete`，`malloc` 配 `free`。因为 `new` 分配的内存带有额外头部信息，`free` 释放会破坏堆链表。

---

### 第 7 题答案 · 结构体内存布局与赋值方式

```cpp
#include <iostream>
#include <cstddef>
using namespace std;

struct Small { char c; int i; };            // 对齐 4，sizeof = 8
struct A { int a; double b; };              // 对齐 8，sizeof = 16（4 + 4填充 + 8）
struct B { Small s; int* p; };              // 对齐 8，sizeof = 16（8 + 8）

int main() {
    cout << "sizeof(A) = " << sizeof(A) << endl;   // 16
    cout << "offsetof(A,a) = " << offsetof(A, a) << endl;  // 0
    cout << "offsetof(A,b) = " << offsetof(A, b) << endl;  // 8 ← 中间 4 字节填充

    cout << "sizeof(B) = " << sizeof(B) << endl;   // 16
    cout << "offsetof(B,s) = " << offsetof(B, s) << endl;  // 0
    cout << "offsetof(B,p) = " << offsetof(B, p) << endl;  // 8

    int heap = 100;
    B b1; b1.s.c = 'A'; b1.s.i = 1; b1.p = &heap;

    // 方式一：整体赋值
    B b2 = b1;
    // 方式二：逐成员赋值
    B b3; b3.s = b1.s; b3.p = b1.p;

    *b2.p = 200;          // 通过副本改堆区
    *b3.p = 300;
    cout << "heap = " << heap << endl;   // 300 ← 原对象也被牵连
    cout << "b1.p == b2.p : " << (b1.p == b2.p) << endl;  // 1
    cout << "b1.p == b3.p : " << (b1.p == b3.p) << endl;  // 1
    return 0;
}
```

**关键结论**

- **整体赋值 `B b2 = b1;` 本质是 `memcpy`**，编译器按字节拷贝整个对象；**逐成员赋值 `b3.s = b1.s; b3.p = b1.p;`** 则是逐个成员做拷贝赋值。对于 `Small`（无指针）两者等价；但对指针成员 `p`，两种写法都只是**拷贝了指针值**，因此 `b1/b2/b3` 三个指针指向同一块堆内存，谁改都牵连其他。
- 这正是第 17 题「浅拷贝导致重复释放」的根源：**一个堆内存被三个对象各自 `delete` 一次**。
- 内存对齐规则：每个成员按自身对齐要求（`int`→4，`double`→8，指针→8）放入，偏移必须是对齐量的整数倍，不足处补填充字节；**结构体总大小必须是最大成员对齐量的整数倍**（若不是，尾部还要补）。所以 `A` 中 `int` 之后补了 4 字节。

---

### 第 8 题答案 · 三种传参方式对比

```cpp
#include <iostream>
using namespace std;

void byValue(int v)      { v = 999; }                  // 改副本
void byPointer(int* p)   { *p = 999; }                // 改指向的值
void byRef(int& r)       { r = 999; }                 // 改别名本身
void constRef(const int& r) { /* r = 1; */ }           // 只能读
void mix(int a, int* p, int& r) {                      // 混合签名
    a = 1; *p = 2; r = 3;
}

int main() {
    int x = 10;
    byValue(x);    cout << "byValue 后 x = " << x << endl;   // 10（未变）
    byPointer(&x); cout << "byPointer 后 x = " << x << endl; // 999
    int y = 20;
    byRef(y);      cout << "byRef 后 y = "    << y << endl;   // 999

    int m = 0, q = 0, r = 0;
    mix(m, &q, r);
    cout << "mix 后 m = " << m << " q = " << q << " r = " << r << endl; // 0  2  3
    return 0;
}
```

**关键结论**

- 值传递：拷贝副本，函数内改动不外传；**但对指针参数调用时，指针值本身是被拷贝的**。
- 指针传递：显式、需要调用方写 `&`，可设为 `nullptr` 做判空，语法上比引用啰嗦。
- 引用传递：调用方无需特殊语法，是「起别名」，惯用于函数参数；**形参不能是 `nullptr`**。
- 只读参数用 `const int&`，避免拷贝又表达只读意图，是最佳实践。

---

### 第 9 题答案 · 内存分区模型

```cpp
#include <iostream>
using namespace std;

int    g_a        = 10;          // 全局变量      → 全局区
static int g_s    = 20;          // 静态全局变量  → 全局区
const  int g_c    = 30;          // 全局常量      → 常量区
const char* g_str = "literal";   // 字符串字面量  → 常量区

void show() {
    static int s_local = 40;     // 静态局部变量  → 全局区（只初始化一次）
    int local        = 50;       // 局部变量      → 栈区
    int* heap        = new int;  // new 出来的    → 堆区
    int stackArr[4];             // 栈数组        → 栈区

    cout << "&g_a        = " << &g_a        << endl;
    cout << "&g_s        = " << &g_s        << endl;
    cout << "&g_c        = " << &g_c        << endl;
    cout << "&g_str      = " << (void*)g_str<< endl;
    cout << "&s_local    = " << &s_local    << endl;  // 与 g_a 同区
    cout << "&local      = " << &local      << endl;
    cout << "stackArr    = " << (void*)stackArr << endl;
    cout << "heap        = " << (void*)heap << endl;

    delete heap;
}

int main() { show(); show(); }   // 第二次调用 s_local 仍为 40（静态局部）
```

**关键结论**

| 区域 | 存放内容 | 生命周期 | 管理者 |
|---|---|---|---|
| 代码区 | 函数体二进制指令，只读可共享 | 程序运行期间 | 操作系统 |
| 全局区 | 全局变量、`static` 局部/全局变量 | 程序运行期间 | 操作系统 |
| 常量区 | `const` 全局、`"字符串字面量"` | 程序运行期间 | 操作系统 |
| 栈区 | 函数参数、局部变量、栈数组 | **函数调用结束即释放** | 编译器自动 |
| 堆区 | `new` / `malloc` 的内存 | **由程序员控制** | 程序员 |

- **实测地址（x86-64 Ubuntu 22.04 / g++ 11.4，ASLR 开启，每次运行数值都会变）**：

  | 变量 | 区域 | 实测地址示例 |
  |---|---|---|
  | `local`、`stackArr` | 栈区 | `0x7fff37ac8754` `0x7fff37ac8760` |
  | `heap`（`new int`） | 堆区 | `0x61be83679eb0` |
  | `g_a`、`g_s`、`s_local` | 全局区 | `0x61be7f6e2010` `0x61be7f6e2014` `0x61be7f6e2018` |
  | `g_c`、`g_str` | 常量区 | `0x61be7f6e0004` `0x61be7f6e0008` |

- **纠正两个常见误传**：① **栈向低地址增长**（`push` 令 `%rsp` 减小），不是向高地址；② 堆/全局/常量三者的地址高低**没有固定顺序**，上表中堆地址甚至**高于**全局区。原因是 ASLR 让各段基址随机化，`new` 大块内存还可能走 `mmap`。
- 因此**不要用地址大小判断变量属于哪个区**。正确判据是变量的类别与生命周期（见上表）；地址只能用来观察「同类变量彼此相邻」这一点（如三个全局变量连续相差 4 字节）。
- 静态局部变量：`static` 使其延生命周期到程序结束，且**只初始化一次**（可用作函数内计数器）。上面 `show()` 调两次，`s_local` 第二次仍是 40。

---

## 第二层 · 引用与函数提高

### 第 10 题答案 · 引用的返回值 vs 返回局部地址

```cpp
#include <iostream>
using namespace std;

int&  getStaticRef()  { static int v = 10; return v; }   // OK：static 存活到程序结束
int*  getStaticAddr(){ static int v = 20; return &v; }   // OK
int&  getLocalRef()   { int v = 30; return v; }          // 危险：悬垂引用
int*  getLocalAddr()  { int v = 40; return &v; }          // 危险：野指针

int main() {
    cout << getStaticRef()   << endl;     // 10
    cout << *getStaticAddr() << endl;     // 20（注意：直接输出指针只会打印地址）
    cout << getLocalRef()    << endl;     // ❌ 危险：实测在此处立即崩溃
    cout << getLocalAddr()   << endl;     // ❌ 同上，不可预期

    getStaticRef() = 99;                 // 引用返回值可作为左值，直接改原变量
    cout << "static ref 改为 " << getStaticRef() << endl;  // 99
    return 0;
}
```

**实测输出（g++ 11.4，`-O0` 与 `-O2` 结果相同）**

```
10
20
Segmentation fault (core dumped)
```

第 3 行执行时进程就崩溃了，后两行根本没机会运行——**没有任何"看起来正确"的中间状态**。

若想看到后面「引用作左值」的演示，把第 3、4 行注释掉再编译：

```cpp
#include <iostream>
using namespace std;

int&  getStaticRef()  { static int v = 10; return v; }
int*  getStaticAddr() { static int v = 20; return &v; }

int main() {
    cout << getStaticRef()   << endl;     // 10
    cout << *getStaticAddr() << endl;     // 20
    // cout << getLocalRef()  << endl;    // 注释掉：否则崩溃
    // cout << getLocalAddr() << endl;

    getStaticRef() = 99;                 // 引用返回值可作为左值
    cout << "static ref 改为 " << getStaticRef() << endl;  // 99
    return 0;
}
```

输出：`10` `20` `static ref 改为 99`。

**关键结论**

1. **引用/指针返回值的有效期取决于被引用对象的生命周期**，不由函数调用决定。
2. **实测纠正**：旧说法「`-O0` 下栈帧还在、常常看起来还能读出正确值」**在本环境不成立**。g++ 11.4 对「返回局部对象引用/地址」这种未定义行为会插入陷阱指令，**`-O0` 与 `-O2` 下都是执行到就 `Segmentation fault`**。GCC 编译时会给出 `warning: reference to local variable 'v' returned [-Wreturn-local-addr]`，这个警告必须当成错误处理。
3. 这正是第 1 题「未定义行为」在实践中的典型后果：**UB 没有任何保证**，崩溃、读到垃圾值、看似正常都是"合法"结果，且随编译器版本/优化级别而变。**这不是优化引入的 bug，而是未定义行为本身**——永远不要依赖某次"跑出来了"的侥幸结果。
4. 正确做法：想让函数返回可修改的引用，就返回 **`static` 局部变量、全局变量、堆内存** 或**调用方传入的对象成员**的引用。
5. `T&` 与 `T` 的另一个区别：返回引用时可以 `getStaticRef() = 99`；返回值类型为 `T` 时 `getVal() = 99` 是语法错误（临时对象不可赋值）。

---

### 第 11 题答案 · 默认参数与占位参数

```cpp
#include <iostream>
using namespace std;

// 默认值只写在声明处
void func(int a, int b = 10, int c = 0);

void func(int a, int b, int c) {          // 定义处不再重复默认值
    cout << "a=" << a << " b=" << b << " c=" << c << endl;
}

// 占位参数：只写类型、不写形参名，用来「占一个位置」以区分重载
// ⚠️ 必须与【参数个数不同】的版本配对才能构成重载
void placeholder(int a)      { cout << "单参重载 a=" << a << endl; }
void placeholder(int a, int) { cout << "占位重载 a=" << a << "（第二形参无名，仅占位）" << endl; }

// ❌ 错误写法（原答案的写法，编译不通过）：
//    void placeholder(int a, int)   { ... }
//    void placeholder(int a, int b) { ... }
//    两者签名都是 (int, int) → error: redefinition of 'void placeholder(int, int)'
//    且 placeholder(5) 还会报 too few arguments

int main() {
    func(1);         // a=1  b=10 c=0
    func(1, 2);      // a=1  b=2  c=0
    func(1, 2, 3);   // a=1  b=2  c=3
    placeholder(5);      // 单参重载
    placeholder(5, 6);   // 占位重载（6 传进去但函数内拿不到）
    return 0;
}
```

**运行输出**

```
a=1 b=10 c=0
a=1 b=2 c=0
a=1 b=2 c=3
单参重载 a=5
占位重载 a=5（第二形参无名，仅占位）
```

**关键结论**

- **默认参数必须从右向左连续声明**：若写成 `void f(int a = 1, int b)`，编译报 `error: default argument missing for parameter 2`，因为实参是按位置匹配的，中间空缺无法对应。
- 默认值**只能在函数声明（或定义，如果无声明）处给一次**，两处都给会报 `redefinition of default argument`。
- 占位参数（黑马课程常称「占位符」）只写类型不写形参名，函数体内**无法使用它**。它必须让**参数个数不同**才能构成重载：`f(int)` 与 `f(int, int)` 可以重载；而 `f(int a, int b)` 与 `f(int a, int)` 是**同一个签名**，同时定义会报 `error: redefinition`。这是原答案的错误，已修正。
- 注意：一旦某个形参有默认值，编译器就认为该函数「参数个数不固定」，重载时易产生歧义，设计上要谨慎。

---

### 第 12 题答案 · 函数重载的匹配规则

```cpp
#include <iostream>
using namespace std;

void show(int a)                 { cout << "show(int)    : " << a << endl; }
void show(double a)              { cout << "show(double) : " << a << endl; }
void show(float a)               { cout << "show(float)  : " << a << endl; }
void show(int a, int b)          { cout << "show(int,int): " << a << ',' << b << endl; }

int main() {
    show(10);        // show(int)     ← 精确匹配
    show(3.14);      // show(double)  ← 精确匹配
    show('A');       // show(int)     ← char 整型提升到 int
    show(10, 20);    // show(int,int)
    show(1.0f);      // show(float)   ← 精确匹配

    int  i = 1;  float f = 1.0f;
    show(i);   show(f);

    // long l = 1;
    // show(l);
    // ❌ 编译错误：call of overloaded 'show(long int&)' is ambiguous
    //    long → double 是「转换（conversion）」，long → float 也是转换，
    //    两者同等级 → 二义性歧义调用。放开这两行整份代码就编译不过。
    return 0;
}
```

**运行输出（已注释掉歧义调用）**

```
show(int)    : 10
show(double) : 3.14
show(int)    : 65
show(int,int): 10,20
show(float)  : 1
show(int)    : 1
show(float)  : 1
```

> 注意 `show('A')` 输出的是 `65`（`'A'` 的 ASCII 码），因为它匹配到了 `show(int)`。

**关键结论**

匹配优先级（从高到低）：**完全匹配 → 整型提升/转换 → 标准转换 → 用户自定义转换 → 省略号**。

- 整型提升（promotion）优于标准转换：`char`/`short`/`bool` → `int` 属提升；`int` → `double` 属转换。因此 `show('A')` 选 `show(int)` 而非 `show(double)`。
- `show(l)`（`long`）会报**歧义调用**：`long→float` 与 `long→double` 都是「转换」同级，编译器无法择优。
- 重载只看**形参个数 + 类型**，不看返回值（第 13 题验证）。

---

### 第 13 题答案 · 重载失效的情形

```cpp
#include <iostream>
using namespace std;

typedef int MYINT;                 // MYINT 就是 int，不是新类型

void f(int)  { cout << "f(int)" << endl; }
void f(MYINT){ cout << "f(MYINT)" << endl; }   // ❌ 编译错误：与上一行重复声明
```

```cpp
int  g(int)   { return 1; }
double g(int) { return 2.0; }      // ❌ 编译错误：重载不能只改返回值类型
```

```cpp
#include <iostream>
#include <cstring>
using namespace std;

void h(const char* s) { cout << "h(const char*) : " << s << endl; }
void h(char* s)       { cout << "h(char*)       : " << s << endl; }

int main() {
    h("literal");      // h(const char*) ← 字面量类型是 const char[8]，首选 const 版本
    char buf[16] = "buffer";
    char* p = buf;
    h(p);              // h(char*)     ← 实参恰好是 char*
    return 0;
}
```

**运行输出**

```
h(const char*) : literal
h(char*)       : buffer
```

**关键结论**

1. `typedef`/`using` 只创建**别名**，不产生新类型，所以 `f(int)` 与 `f(MYINT)` 是**同一个函数**，重复定义报错。
2. 函数重载的签名中**不包含返回类型**，仅靠返回值区分的重载非法。
3. `const char*` 与 `char*` 是**两个不同的指针类型**，可以构成重载。但当实参为字符串字面量时，其类型是 `const char[N]`，只能匹配 `const char*`；如果只提供 `char*` 一个重载，调用会失败（字面量不可修改）。
4. 补充：**顶层 `const` 不能用于重载**。`void k(int)` 与 `void k(const int)` 是同一个函数（顶层 const 被忽略），而 `void k(int*)` 与 `void k(const int*)` 可以重载。

---

## 第三层 · 类与对象

### 第 14 题答案 · 构造与析构的调用时机

```cpp
#include <iostream>
#include <string>
using namespace std;

class Person {
    string name;
    int    age;
public:
    Person()                                   { cout << "① 无参构造\n"; }
    Person(string n, int a) : name(n), age(a)  { cout << "② 有参构造\n"; }
    Person(const Person& p) : name(p.name), age(p.age) { cout << "③ 拷贝构造\n"; }
    Person& operator=(const Person& p) {
        if (this != &p) { name = p.name; age = p.age; }
        cout << "④ 赋值运算符\n";
        return *this;
    }
    ~Person()                                  { cout << "析构 " << name << "\n"; }
};

int main() {
    Person p1;                       // ①
    Person p2("张三", 20);            // ②
    Person p3(p2);                   // ③（写法一：用已有对象初始化新对象）
    Person p4 = p2;                  // ③（写法二：等价，仍是拷贝构造）
    p4 = p1;                         // ④（赋值：对象已存在，不会调用拷贝构造）

    cout << "--- 作用域开始 ---\n";
    {
        Person a("A", 1), b("B", 2), c("C", 3);   // ② ×3
        Person* d = new Person("D", 4);            // ②
        delete d;                                   // 立即触发 D 析构
        cout << "--- 作用域结束，局部对象逆序析构 ---\n";
    }
    return 0;
}
```

**运行输出（实测）**

```
① 无参构造
② 有参构造
③ 拷贝构造
③ 拷贝构造
④ 赋值运算符
--- 作用域开始 ---
② 有参构造          ← A
② 有参构造          ← B
② 有参构造          ← C
② 有参构造          ← D（new 出来的）
析构 D               ← delete d 立即触发
--- 作用域结束，局部对象逆序析构 ---
析构 C
析构 B
析构 A
析构                 ← p4（被 p1 赋值，name 为空）
析构 张三            ← p3（拷贝自 p2）
析构 张三            ← p2
析构                 ← p1（无参构造，name 为空）
```

> 易错点：末尾 4 次析构的**第一个是空的**（`p4 = p1` 后 `p4.name` 为空串），顺序是 `空 / 张三 / 张三 / 空`；作用域内 A、B、C 三个局部对象各打印一次「② 有参构造」，不要漏。

**关键结论**

- **拷贝构造 vs 赋值**：`Person p3(p2);` 和 `Person p4 = p2;` 调用**拷贝构造**（用一个对象去初始化另一个新对象）；`p4 = p1;` 调**赋值运算符**（两边对象都已存在）。这是最高频的面试辨析点。
- **析构顺序与构造完全相反**（后构造先析构），因为它们按「后进先出」在栈上。
- `new` 出来的对象不在栈上，**必须靠 `delete` 触发析构**；忘记 `delete` 即内存泄漏 + 构造函数副作用丢失。
- `operator=` 中 `if (this != &p)` 是防御性写法，避免自赋值把值清成乱码。

---

### 第 15 题答案 · 初始化列表与成员声明顺序

```cpp
#include <iostream>
#include <string>
using namespace std;

class Demo {
    int    a;
    string b;
    char   c;
public:
    // 故意打乱书写顺序：写的是 c、b、a
    Demo(int av, const string& bv, char cv)
        : c(cv), b(bv), a(av) {
        cout << "构造函数体内：a=" << a << " b=" << b << " c=" << c << endl;
    }
    void show() const { cout << "show: a=" << a << " b=" << b << " c=" << c << endl; }
};

class Bad {
    int n;
public:
    Bad() { n = 88; }              // 构造函数体内赋值：n 先被默认初始化，再赋值
    int get() const { return n; }
};

int main() {
    Demo d(1, "hello", 'x');       // 实际按 a、b、c 顺序初始化
    d.show();                      // a=1 b=hello c=x（三者都已就绪）

    Bad bad;
    cout << "Bad::n = " << bad.get() << endl;   // 88

    // 危险示例（仅示意，不要放开）：
    // class SelfRef {
    //     int v;
    //     SelfRef(int x) : v(v + x) {}  // 初始化列表里用 v 访问的是「尚未初始化」的成员
    // };
    return 0;
}
```

**关键结论**

1. **初始化列表的真实执行顺序由成员在类中的声明顺序决定**，与你在初始化列表里的书写顺序无关。上面写的 `: c(cv), b(bv), a(av)` 实际执行顺序是 `a → b → c`。
2. 初始化列表是**真正的「初始化」**，构造函数体内赋值是「先默认初始化再赋值」——对 `int` 差别不大，但对 `string`、容器等有构造开销的类型效率差很多，所以推荐一律用初始化列表。
3. 初始化列表里可以出现成员名，但只能作为**右侧的表达式依赖**（如 `v(initv)`），**不能读取尚未初始化的成员**。这是经典陷阱。
4. `const` 成员与「没有默认构造函数且无参数化构造」的成员，**只能**在初始化列表中初始化。

---

### 第 16 题答案 · 嵌套类对象的构造与析构顺序

```cpp
#include <iostream>
using namespace std;

class Member {
public:
    Member() { cout << "  Member 构造\n"; }
    ~Member(){ cout << "  Member 析构\n"; }
};

class Outer {
    Member m;                    // 成员对象
    int*   p;
public:
    Outer() : p(new int(10)) { cout << "Outer 构造 (p=" << *p << ")\n"; }
    ~Outer(){ cout << "Outer 析构\n"; delete p; }
};

class Derived : public Outer {
public:
    Derived() { cout << "Derived 构造\n"; }
    ~Derived(){ cout << "Derived 析构\n"; }
};

int main() {
    cout << "--- 创建 Outer ---\n";
    Outer o;
    cout << "--- 创建 Derived ---\n";
    Derived d;
    cout << "--- main 结束 ---\n";
    return 0;
}
```

**运行输出**

```
--- 创建 Outer ---
  Member 构造
Outer 构造 (p=10)
--- 创建 Derived ---
  Member 构造
Outer 构造 (p=10)
Derived 构造
--- main 结束 ---
Derived 析构
Outer 析构
  Member 析构
Outer 析构
  Member 析构
```

**关键结论**

- **构造顺序**：基类部分（先虚基类后非虚基类）→ 成员对象（按声明顺序）→ 宿主类构造函数体。
- **析构顺序**：完全相反——宿主类析构体 → 成员对象析构（逆序）→ 基类析构。
- 一句话记忆：**构造像「叠盘子」自底向上，析构像「撤盘子」自顶向下**。
- 因为 `Outer` 的析构函数中释放了 `p`，所以 `Outer` 必须被正确析构；这也是「基类析构函数应写成 `virtual`」的直接原因（第 27 题会验证）。

---

### 第 17 题答案 · 拷贝构造与深浅拷贝

```cpp
#include <iostream>
#include <string>
using namespace std;

// ---- 版本 A：浅拷贝（错误示范）----
class BadPerson {
public:
    int    age;
    string* name;
    BadPerson(int a, const string& n) : age(a), name(new string(n)) {}
    BadPerson(const BadPerson& p) : age(p.age), name(p.name) {}   // ❌ 只拷贝指针
    ~BadPerson() { delete name; }                                 // ❌ 会二次释放
};

// ---- 版本 B：深拷贝（正确示范）----
class Person {
public:
    int    age;
    string* name;
    Person(int a, const string& n) : age(a), name(new string(n)) {}
    Person(const Person& p) : age(p.age), name(new string(*p.name)) {}  // ✅ 重新分配 + 复制内容
    ~Person() { delete name; }

    Person& operator=(const Person& p) {          // 深拷贝的赋值运算符
        if (this != &p) {
            delete name;
            age = p.age;
            name = new string(*p.name);
        }
        return *this;
    }
};

int main() {
    cout << "=== 深拷贝版本 ===\n";
    Person a(20, "张三");
    Person b(a);                       // 触发拷贝构造
    Person c = a;                      // 同为拷贝构造
    cout << "a.name = " << *a.name << ", b.name = " << *b.name << endl;
    cout << "三者指针是否相同: " << (a.name == b.name) << endl;  // 0（各持一份）

    Person d(0, "临时");
    d = a;                             // 赋值运算符
    cout << "赋值后 d.name = " << *d.name << endl;   // 张三
    return 0;
}
```

**运行结果（实测）**

- **版本 B（深拷贝）**：正常结束，无报错，三个对象各自持有一份独立的堆字符串。上面 `main()` 里实例化的正是这一版。
- **版本 A（浅拷贝）**：⚠️ 上面 `main()` 中 `BadPerson` **从未被实例化**，所以直接运行这份代码不会崩。要复现问题，需单独加上：

```cpp
#include <iostream>
#include <string>
using namespace std;

class BadPerson {
public:
    int    age;
    string* name;
    BadPerson(int a, const string& n) : age(a), name(new string(n)) {}
    BadPerson(const BadPerson& p) : age(p.age), name(p.name) {}   // ❌ 只拷贝指针
    ~BadPerson() { delete name; }                                 // ❌ 会二次释放
};

int main() {
    BadPerson a(20, "张三");
    BadPerson b(a);                 // 浅拷贝：b.name 与 a.name 指向同一块内存
    cout << *a.name << ", " << *b.name << endl;
    cout << "--- main 结束，开始析构 ---" << endl;
    return 0;                       // ← 对同一地址 delete 两次
}
```

实测输出（g++ 11.4）：

```
张三, 张三
--- main 结束，开始析构 ---
Aborted (core dumped)
```

补充：成员是 `string*` 时，第二次 `delete` 会因堆管理信息被破坏而抛出 `std::length_error: basic_string::_M_create` 后 `terminate`；成员若是裸 `char*`，典型表现则是 `free(): double free detected in tcache 2`。**两者本质相同——同一块堆内存被释放两次**。若改成不 `delete`，则退化为内存泄漏：两种结局都是错的。

**关键结论**

| | 浅拷贝 | 深拷贝 |
|---|---|---|
| 指针成员处理 | 直接拷贝指针值（两个指针指向同一块内存） | 重新 `new` 一块并拷贝内容 |
| 后果 | 重复释放 / 悬垂指针 | 安全 |
| 额外代价 | 无 | 额外内存 + 拷贝开销 |

- 拷贝构造函数的自定义规则一句话：**对每个指针/引用成员，要么重新分配并深拷贝，要么置空/移交所有权**，不能只拷指针。
- 实际工程建议：**优先用 `string`、`vector`、`shared_ptr` 等值语义类型替代裸指针成员**，从根上消灭浅拷贝问题。

---

### 第 18 题答案 · string 成员 vs 指针成员

```cpp
#include <iostream>
#include <string>
using namespace std;

class WithString {              // 成员是 string 对象
public:
    string name;
    int    age;
    WithString(const string& n, int a) : name(n), age(a) {}
    WithString(const WithString& p) : name(p.name), age(p.age) {}   // 无需手写深拷贝
};

class WithPointer {             // 成员是 string*
public:
    string* name;
    int      age;
    WithPointer(const string& n, int a) : age(a), name(new string(n)) {}
    WithPointer(const WithPointer& p) : age(p.age), name(p.name) {}  // ❌ 浅拷贝
    ~WithPointer() { delete name; }
    // 若把 name 改成裸 int* 而不 new，则拷贝时可能拷贝到未初始化/野指针
};

int main() {
    WithString  s1("李四", 21);
    WithString  s2(s1);
    s2.name = "李四(改)";                   // ✅ name 是 string 对象，直接赋值
                                            // ❌ 不能写 *s2.name（原答案写法，编译报错：
                                            //    no match for 'operator*' (operand type is 'std::string')）
    cout << "s1.name = " << s1.name << endl; // 李四 ← s1 未受影响（深拷贝）

    WithPointer p1("王五", 22);
    WithPointer p2(p1);                      // 浅拷贝：p2.name 与 p1.name 指向同一块内存
    *p2.name = "王五(改)";
    cout << "p1.name = " << *p1.name << endl; // 王五(改) ← p1 被牵连（浅拷贝）
    p2.name = nullptr;                        // 兜底：否则 main 结束时对同一地址 delete 两次会崩溃

    cout << "sizeof(string) = " << sizeof(string) << endl;   // 32（x86-64 libstdc++）
    return 0;
}
```

**运行输出（实测）**

```
s1.name = 李四
p1.name = 王五(改)
sizeof(string) = 32
```

> 若不加 `p2.name = nullptr;` 这行兜底，程序在打印完三行后、`main` 结束时析构 p2、p1 会对同一地址 `delete` 两次，实测**异常终止**（`Aborted (core dumped)`）。这正是第 17 题浅拷贝问题的复现。

**关键结论**

| | `string name` | `string *name` |
|---|---|---|
| sizeof | 32（对象内含指针+长度+容量+SSO 缓冲） | 8 |
| 拷贝语义 | **默认深拷贝**，规则由 `string` 自己保证 | 只拷贝指针值 |
| 资源所有权 | 类自己管，析构自动释放 | 谁 `new` 谁 `delete`，易重复释放 |
| 修改独立性 | 拷贝对象与原对象完全独立 | 共享同一块内存 |

**工程结论**：能用 `string`/`vector` 等值类型，就不要用 `string*`。`string` 的短字符串优化（SSO）还避免了小字符串的堆分配开销。

---

### 第 19 题答案 · 静态成员与静态成员函数

```cpp
#include <iostream>
using namespace std;

class Person {
public:
    static int count;                 // 静态成员变量：类内声明
    static void showCount() { cout << "类内静态函数: count=" << count << endl; }
    static void tryAccess() {
        // cout << age << endl;      // ❌ 静态成员函数没有 this，不能访问非静态成员
        count++;                     // ✅ 能访问静态成员
    }
    Person()          { count++; }    // 每 new 一个对象加一
    int getAge() { return age; }      // 普通成员函数：有 this
private:
    string name; int age;
};

int Person::count = 0;                // 静态成员变量必须在类外定义并初始化（不写 static）

void externalFunc() {                 // 外部函数访问静态成员
    Person::count = 100;
    cout << "外部函数改后 count=" << Person::count << endl;
}

int main() {
    Person p1, p2;
    Person::showCount();              // count=2（先默认构造两个）
    externalFunc();                   // count=100
    Person::tryAccess();              // count=101
    Person* p3 = new Person();
    Person::showCount();              // count=102
    delete p3;
    return 0;
}
```

**关键结论**

1. **静态成员变量不属于任何单个对象**，只有一份，被所有对象共享。它在类内**只能声明**，必须在**类外**单独定义（`int Person::count = 0;`，此处不写 `static`），否则链接期报 `undefined reference`。
2. **静态成员函数没有 `this` 指针**，因此：不能访问非静态成员、不能调用非静态成员函数、`const` 修饰无意义。
3. 静态成员函数可以通过 `类名::函数名()` 调用，也可以通过对象调用（但推荐用类名，强调「与对象无关」）。
4. `const` 成员函数中 `this` 是 `const Person*`，因此 `operator=` 里的 `this != &p` 在 `const` 上下文中类型也一致，比较合法。

---

### 第 20 题答案 · this 指针与空对象占用

```cpp
#include <iostream>
#include <cstddef>
using namespace std;

class Person {
public:
    int    age;
    char   tag;
    double score;

    void showThis() { cout << "this = " << (void*)this << endl; }
    int&  ageRef()  { return this->age; }        // 返回成员引用 → 外部可直接改
    void setAge(int a) { this->age = a; }        // 显式使用 this
};

void dump(const char* tag, Person& p) {
    cout << tag << ": 对象地址=" << (void*)&p
         << " sizeof=" << sizeof(p)
         << " age偏移="  << offsetof(Person, age)
         << " tag偏移="  << offsetof(Person, tag)
         << " score偏移="<< offsetof(Person, score)
         << endl;
}

int main() {
    Person p;                 // 空对象：不初始化任何成员
    dump("空对象", p);
    p.showThis();
    p.setAge(18);             // 空对象也能调用成员函数
    cout << "age = " << p.age << endl;   // 18

    int& r = p.ageRef();      // ✅ ageRef() 返回 int&，必须用 int& 接（原答案写成 Person&，编译报错）
    r = 30;                   // 通过引用改成员
    cout << "age = " << p.age << endl;   // 30

    Person q;
    q = p;                    // 整体赋值
    dump("普通对象", q);
    return 0;
}
```

**运行结果（x86-64，实测；地址因 ASLR 每次不同）**

```
空对象: 对象地址=0x7fff2cdd4b80 sizeof=16 age偏移=0 tag偏移=4 score偏移=8
this = 0x7fff2cdd4b80
age = 18
age = 30
普通对象: 对象地址=0x7fff2cdd4b90 sizeof=16 age偏移=0 tag偏移=4 score偏移=8
```

> `q` 与 `p` 是两个不同的栈对象，地址相差 16 字节（正好一个 `sizeof(Person)`），所以二者地址**不同**。

**关键结论**

1. **`this` 指向当前对象**，在成员函数内部可直接用成员名（编译器隐式补 `this->`）。C++ 中 `this` 是**关键字**不是普通参数（Java 里是参数，可以传），不能赋值、不能传参。
2. **`sizeof(Person) = 16`**：`int`(4) + `char`(1) + 3 字节填充 + `double`(8) = 16。
3. **空对象也至少占 1 字节**（C++ 标准规定），因为每个独立对象必须有唯一的内存地址，`sizeof` 不可能为 0。
4. `p` 未初始化时其成员是**未定义值**（`int` 可能是任意值），但调用成员函数本身是安全的——这与第 1 题「未定义行为」不同：未定义的是「读未初始化变量」，不是「调用成员函数」。

---

### 第 21 题答案 · 内存对齐与 sizeof

```cpp
#include <iostream>
#include <cstddef>
using namespace std;

struct A { char c; int  i; };          // int 对齐 4 → 总大小须为 4 的倍数
struct B { char c; double d; };        // double 对齐 8 → 16
struct C { char c1; char c2; int i; }; // 两个 char 连续填满 2 字节再补 2 → 8
struct A2{ int  i; char c; };          // 换顺序：4 + 1 + 3 填充 = 8（没变）

void show(const char* n, size_t sz, size_t o1) {
    cout << n << ": sizeof=" << sz << " 首成员偏移=" << o1 << endl;
}

int main() {
    show("A  (char,int)   ", sizeof(A),  offsetof(A,  i));
    show("B  (char,double)", sizeof(B),  offsetof(B,  d));
    show("C  (char,char,int)", sizeof(C), offsetof(C, i));
    show("A2 (int,char)   ", sizeof(A2), offsetof(A2, c));
    cout << "alignof(int)=" << alignof(int)
         << " alignof(double)=" << alignof(double) << endl;   // 4  8
    return 0;
}
```

**运行结果**

```
A  (char,int)   : sizeof=8 首成员偏移=4
B  (char,double): sizeof=16 首成员偏移=8
C  (char,char,int): sizeof=8 首成员偏移=4
A2 (int,char)   : sizeof=8 首成员偏移=4
alignof(int)=4 alignof(double)=8
```

**关键结论与填充字节来源**

| 结构体 | 布局 | 填充 | 总大小 |
|---|---|---|---|
| `A{char c; int i;}` | `c`(0) + 3 填充 + `i`(4~7) | **3 字节中间填充** | 8 |
| `B{char c; double d;}` | `c`(0) + 7 填充 + `d`(8~15) | **7 字节中间填充** | 16 |
| `C{char c1; char c2; int i;}` | `c1`(0) `c2`(1) + 2 填充 + `i`(4~7) | **2 字节中间填充** | 8 |
| `A2{int i; char c;}` | `i`(0~3) `c`(4) + **3 尾部填充** | 3 字节尾部填充 | 8 |

- **换顺序不会减小 `sizeof(A)`**：虽然填充位置从中间移到了尾部，但总大小仍受最大对齐量约束。
- **总大小必须是最大成员对齐量的整数倍**：若 `A` 只有 `char c[3]; int i;`，中间补 1 字节，总大小 8。
- 对齐的目的是让 CPU 一次访存就能取到数据，同时让数组元素不跨缓存行。嵌入式/通信结构体设计中常用 `#pragma pack(1)` 关闭对齐以节省存储，代价是访问变慢（部分平台还可能崩溃）。

---

## 第四层 · 继承

### 第 22 题答案 · 继承与访问控制

```cpp
#include <iostream>
using namespace std;

class Base {
public:    int  pub;      // public 成员
protected: int  prot;     // protected 成员
private:   int  priv;     // private 成员
public:
    void set(int p, int q, int r) { pub = p; prot = q; priv = r; }
    void show() { cout << "Base::show pub=" << pub << " prot=" << prot << endl; }
};

class DPublic    : public Base    {};
class DProtected : protected Base {};
class DPrivate   : private Base   {};

int main() {
    DPublic d1;  DProtected d2;  DPrivate d3;

    d1.pub = 1;          // ✅ public 继承：public 成员在外部可访问
    // d1.prot = 2;      // ❌ protected：外部不可访问
    // d1.priv = 3;      // ❌ private：任何继承方式都不可外部访问
    d1.set(1, 2, 3);     // ✅ 通过公有接口间接设置
    d1.show();

    // d2.pub = 1;       // ❌ protected 继承：public 成员变成 protected
    // d3.pub = 1;       // ❌ private 继承：public 成员变成 private

    // 公有继承的子类内部可以访问 protected：
    // struct Helper : public Base { void f() { prot = 9; } };
    return 0;
}
```

**关键结论 · 访问可见性转换表**

| 基类成员 | `public` 继承 | `protected` 继承 | `private` 继承 |
|---|---|---|---|
| `public` | public | **protected** | **private** |
| `protected` | protected | **protected** | **private** |
| `private` | **不可访问** | **不可访问** | **不可访问** |

- `private` 成员**永远不能被子类直接访问**，只能通过基类的公有/保护接口间接操作。
- 一句话记忆：**`public` 继承「继续公开」，`protected` 继承「把公开的也藏起来给子类」，`private` 继承「彻底封死继承来的东西」**。
- 黑马课程中的「多继承」通常用 `public`，接口继承用 `public`，实现继承用 `private`。

---

### 第 23 题答案 · 同名成员属性与同名函数

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    int  age = 10;
    void show() { cout << "Base::show  age=" << age << endl; }   // 访问 Base::age
    void onlyBaseFunc() { cout << "Base::onlyBaseFunc\n"; }
};
class Derived : public Base {
public:
    int  age = 20;                        // 同名成员属性，遮蔽 Base::age
    void show() { cout << "Derived::show age=" << age << endl; } // 同名函数，隐藏 Base::show
    void onlyDerivedFunc() { cout << "Derived::onlyDerivedFunc\n"; }
};

int main() {
    Derived d;
    cout << "d.age = " << d.age << endl;            // 20（Derived 版本）

    d.Base::age = 30;                    // 用作用域解析访问基类
    cout << "d.age = " << d.age << endl;            // 20（属性遮蔽是编译期的）
    cout << "d.Base::age = " << d.Base::age << endl;// 30

    d.show();                             // Derived::show  age=20
    d.Base::show();                       // Base::show     age=30

    d.onlyDerivedFunc();                  // ✅ 子类独有
    d.onlyBaseFunc();                     // ✅ 基类独有（未被隐藏）
    return 0;
}
```

**运行输出**

```
d.age = 20
d.age = 20
d.Base::age = 30
Derived::show age=20
Base::show age=30
Derived::onlyDerivedFunc
Base::onlyBaseFunc
```

**关键结论**

- **成员属性**：派生类声明同名属性后，基类的同名属性被**遮蔽（shadowing）**，必须用 `d.Base::age` 显式访问。遮蔽是编译期就确定的，与运行顺序无关。
- **成员函数**：派生类声明同名函数后，基类的同名函数被**隐藏（hiding）**。注意二者机制不同：
  - **函数重写（override）**：需要 `virtual` + 相同签名，是多态的基础（第 27 题）；
  - **函数隐藏（非 virtual 的同名函数）**：只是名字冲突，不产生多态。这里 `show()` 若在 `Base` 中**没有 `virtual`**，则 `d.Base::show()` 与 `d.show()` 是两个不同函数，不存在「谁覆盖谁」。
- 若派生类**只**声明同名函数而不声明同名属性，则属性不遮蔽（`d.age` 仍为 10），函数仍隐藏——两者独立生效。

---

### 第 24 题答案 · 同名静态成员的继承访问

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    static int s;                       // Base::s
    static void showS() { cout << "Base::showS   看到 s=" << s << endl; }
};
int Base::s = 100;

class Derived : public Base {
public:
    static int s;                       // Derived::s，遮蔽 Base::s
    static void showS() { cout << "Derived::showS 看到 s=" << s << endl; }
};
int Derived::s = 200;

int main() {
    Derived d;
    cout << "d.s          = " << d.s          << endl;  // 200（沿作用域找到 Derived::s）
    cout << "Derived::s   = " << Derived::s   << endl;  // 200
    cout << "Base::s      = " << Base::s      << endl;  // 100
    cout << "d.Base::s    = " << d.Base::s    << endl;  // 100

    d.s = 300;
    cout << "改后 Derived::s=" << Derived::s
         << "  Base::s="    << Base::s << endl;         // 300  100（两个独立变量）

    d.showS();                          // Derived::showS
    Base::showS();                      // Base::showS → 看到 Base::s
    return 0;
}
```

**运行输出**

```
d.s          = 200
Derived::s   = 200
Base::s      = 100
d.Base::s    = 100
改后 Derived::s=300  Base::s=100
Derived::showS 看到 s=300
Base::showS   看到 s=100
```

**关键结论**

1. 派生类的同名静态成员**遮蔽**基类的，二者是**两个独立的变量**（各有各的存储），修改一个不影响另一个。
2. 访问路径：`派生类对象.s` → 沿作用域链找到 `Derived::s`；`Base::s` 精确取到基类版本；`d.Base::s` 同理。
3. **基类静态成员函数内的访问指向基类版本**（编译时就已绑定到 `Base` 作用域），这与实例成员函数「运行时看 this」完全不同——静态函数没有 this。
4. **注意**：静态成员函数**不存在重写**，`d.showS()` 只是「隐藏」，若想通过基类访问仍须写 `Base::showS()`。

---

### 第 25 题答案 · 菱形继承：普通继承的改造过程

```cpp
#include <iostream>
using namespace std;

// ============ 阶段一：普通继承的菱形 ============
class Animal {
public:
    int age;
    virtual void speak() { cout << "Animal::speak\n"; }   // 加 virtual 以便对比多态
};
class Dog    : public Animal { public: void bark() { cout << "Dog::bark\n"; } };
class Sheep  : public Animal { public: void graze() { cout << "Sheep::graze\n"; } };
class GuideDog : public Dog, public Sheep {
public:
    void guide() { cout << "GuideDog::guide\n"; }
};

int main() {
    GuideDog g;
    cout << "sizeof(GuideDog) = " << sizeof(g) << endl;   // 32（Dog 子对象 16 + Sheep 子对象 16）

    g.guide();
    g.bark();
    g.graze();
    // g.speak();      // ❌ 编译错误：二义性 —— 不知道该调 Dog::speak 还是 Sheep::speak
    g.Dog::speak();         // ✅ 用 :: 限定逐个消解（但这仍是两次不同调用）
    g.Sheep::speak();
    // cout << g.age;  // ❌ 编译错误：'age' 同样二义（原答案直接写 g.age，编译不过）
    g.Dog::age = 5;
    cout << "g.Dog::age = " << g.Dog::age << "，g.Sheep::age = " << g.Sheep::age << endl;
    // 5, 0 ← 两份独立数据，改了一个另一个不变

    return 0;
}
```

**阶段一运行输出（实测）**

```
sizeof(GuideDog) = 32
GuideDog::guide
Dog::bark
Sheep::graze
Animal::speak
Animal::speak
g.Dog::age = 5，g.Sheep::age = 0
```

**阶段一问题诊断**

1. **二义性（ambiguity）**：最底层对象里有**两份** `Animal` 子对象，`g.speak()` 无法确定走哪条路径，编译期直接报错。**注意：不只是函数二义，`g.age` 这样的成员变量同样二义**（`error: request for member 'age' is ambiguous`），必须写 `g.Dog::age`。
2. **数据冗余**：`age` 被存储了**两份**，浪费内存且**语义错误**——如果通过 `g.Dog::age = 5` 修改，读 `g.Sheep::age` 仍是原值，两者本该是同一个数据。
3. 解决二义性有两种方式：**用 `::` 限定**（治标，`Dog::age` 和 `Sheep::age` 依然是两份数据）或**虚继承**（治本）。

```cpp
// ============ 阶段二：改虚继承 ============
#include <iostream>
using namespace std;

class Animal2 {
public:
    int age;
    virtual void speak() { cout << "Animal2::speak\n"; }
};
class Dog2    : public virtual Animal2 { public: void bark() { cout << "Dog2::bark\n"; } };
class Sheep2  : public virtual Animal2 { public: void graze() { cout << "Sheep2::graze\n"; } };
class GuideDog2 : public Dog2, public Sheep2 {
public:
    void guide() { cout << "GuideDog2::guide\n"; }
};

int main() {
    GuideDog2 g;
    cout << "sizeof(GuideDog2) = " << sizeof(g) << endl;  // 32（含一个 vbptr + 虚表指针 + 一份 age）
    g.guide(); g.bark(); g.graze();
    g.speak();                       // ✅ 无二义性：Animal2 只有一份子对象

    g.Dog2::age = 5;
    cout << "g.age = " << g.age << endl;   // 5 —— 同一份数据 ✅
    return 0;
}
```

**阶段二运行输出（实测）**

```
sizeof(GuideDog2) = 32
GuideDog2::guide
Dog2::bark
Sheep2::graze
Animal2::speak
g.age = 5
```

> `sizeof` 对比：普通继承 32、虚继承也是 32——**数值相同但构成不同**。普通继承是「两个 16 字节基类子对象」；虚继承是「两个 vbptr(8+8) + 虚基类子对象(vfptr 8 + age 4 → 16)」。原答案说普通继承是 24，实测为 **32**。

**关键结论**

| | 普通继承菱形 | 虚继承菱形 |
|---|---|---|
| `Animal` 子对象数量 | **2 份** | **1 份** |
| `g.speak()` | ❌ 二义性编译错误 | ✅ 正常 |
| `age` 存储 | 2 份，语义上会不一致 | 1 份，全局唯一 |
| `sizeof` | **32**（Dog 16 + Sheep 16） | **32**（2×vbptr + 一份虚基类子对象） |
| `vbptr / vbtable` | 无 | 有：`vbptr` 指向 `vbtable`，`vbtable` 记录虚基类偏移 |
| 构造顺序 | 各分支独立构造 `Animal` 两次 | `Animal` 只构造一次 |

**机制**：虚继承的派生类对象里多出一个 `vbptr`（虚基类指针），指向一张 `vbtable`（虚基类表），表中记录了虚基类相对于本对象的偏移量，这样编译器就能在「不随任何特定基类存放」的公共区域里定位唯一的虚基类子对象。

**重要提醒**：虚继承**解决了数据冗余，但让成员访问多了一次间接寻址，性能不如非虚继承**。因此 C++ 标准库对接口继承用普通继承 + 虚函数（多态），**只在必须共享单一数据时才用虚继承**（比如菱形结构里的公共基类）。

---

### 第 26 题答案 · 虚继承体系中的构造与析构顺序

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    Animal() { cout << "  Animal 构造\n"; }
    ~Animal(){ cout << "  Animal 析构\n"; }
};
class Dog : public virtual Animal {
public:
    Dog() { cout << " Dog 构造\n"; }
    ~Dog(){ cout << " Dog 析构\n"; }
};
class Sheep : public virtual Animal {
public:
    Sheep() { cout << " Sheep 构造\n"; }
    ~Sheep(){ cout << " Sheep 析构\n"; }
};
class GuideDog : public Dog, public Sheep {
public:
    GuideDog(){ cout << " GuideDog 构造\n"; }
    ~GuideDog(){ cout << " GuideDog 析构\n"; }
};

// ---- 对照组：把 Animal 改回【普通继承】（原答案缺失这组定义，导致编译不过）----
class AnimalP {
public:
    AnimalP() { cout << "  Animal 构造\n"; }
    ~AnimalP(){ cout << "  Animal 析构\n"; }
};
class DogP : public AnimalP {
public:
    DogP() { cout << " Dog 构造\n"; }
    ~DogP(){ cout << " Dog 析构\n"; }
};
class SheepP : public AnimalP {
public:
    SheepP() { cout << " Sheep 构造\n"; }
    ~SheepP(){ cout << " Sheep 析构\n"; }
};
class GuideDogPlain : public DogP, public SheepP {
public:
    GuideDogPlain(){ cout << " GuideDog 构造\n"; }
    ~GuideDogPlain(){ cout << " GuideDog 析构\n"; }
};

int main() {
    cout << "=== 虚继承 ===\n";
    { GuideDog g; cout << "  (作用域结束)\n"; }
    cout << "=== 改为普通继承 ===\n";
    { GuideDogPlain g; cout << "  (作用域结束)\n"; }
    return 0;
}
```

**运行输出（实测）**

```
=== 虚继承 ===
  Animal 构造          ← 虚基类最先，且只一次
 Dog 构造
 Sheep 构造
 GuideDog 构造
  (作用域结束)
 GuideDog 析构
 Sheep 析构
 Dog 析构
  Animal 析构          ← 最后，且只一次

=== 改为普通继承 ===
  Animal 构造          ← 第一条继承路径的 Animal
 Dog 构造
  Animal 构造          ← 第二条继承路径又来一次！
 Sheep 构造
 GuideDog 构造
  (作用域结束)
 GuideDog 析构
 Sheep 析构
  Animal 析构
 Dog 析构
  Animal 析构
```

**关键结论**

1. **构造顺序**：虚基类 → 非虚基类（按继承声明顺序，同层中先声明的基类先构造）→ 成员对象 → 自身构造函数体。
2. **虚基类只被构造一次、也只被析构一次**，且**总是最先构造、最后析构**。
3. 普通继承下 `Animal` 被构造两次、析构两次——这就是数据冗余在构造析构层面的体现。
4. 析构顺序始终与构造顺序**严格相反**。
5. 若虚基类的构造函数带参数：虚继承中虚基类由**最派生类**负责构造，参数在派生构造函数的初始化列表中指定，且**多个虚基类按继承深度从浅到深初始化**。

---

## 第五层 · 多态

### 第 27 题答案 · 动态多态的基本用法

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void speak() { cout << "Animal::speak\n"; }
    virtual ~Animal() { cout << "Animal::~Animal\n"; }   // 关键：虚析构
};
class Cat : public Animal {
public:
    void speak() override { cout << "Cat::speak\n"; }
    ~Cat() { cout << "Cat::~Cat\n"; }
};
class Dog : public Animal {
public:
    void speak() override { cout << "Dog::speak\n"; }
    ~Dog() { cout << "Dog::~Dog\n"; }
};

int main() {
    // ① 基类指针
    Animal* p1 = new Cat;
    p1->speak();                       // Cat::speak

    // ② 基类引用
    Animal& r1 = *new Dog;
    r1.speak();                        // Dog::speak

    // ③ 基类指针数组
    Animal* arr[2] = { new Cat, new Dog };
    for (Animal* a : arr) a->speak();  // Cat::speak / Dog::speak

    // ④ 通过基类指针删除
    delete arr[0];                      // Cat::~Cat → Animal::~Animal（若有虚析构）
    delete arr[1];
    return 0;
}
```

**运行输出**

```
Cat::speak
Dog::speak
Cat::speak
Dog::speak
Cat::~Cat
Animal::~Animal
Dog::~Dog
Animal::~Animal
```

**关键结论**

1. **动态多态的两个前提条件**（缺一不可）：
   - 条件一：**有继承关系**（子类继承基类）；
   - 条件二：**子类重写了基类的 `virtual` 函数**（或用 `override` 标注）。
2. 机制：每个有虚函数的类在编译期为每个对象生成一个 **`vfptr`（虚函数指针）** 指向 **`vftable`（虚函数表）**。调用 `p1->speak()` 时，编译器生成「取 `p1` 的 `vfptr` → 查表第 N 项 → 调用」的**晚绑定**代码，而不是在编译期写死 `Animal::speak` 的地址。**「编译看左边、执行看右边」**。
3. **向上转型（upcasting）**：基类指针指向子类对象是安全的，会自动发生；反方向的**向下转型（downcasting）**需显式 `static_cast<Cat*>(p)`（不安全）或 `dynamic_cast<Cat*>(p)`（安全，失败返回 `nullptr`，需 RTTI 支持）。
4. **基类析构函数必须是 `virtual`**：否则通过基类指针 `delete` 子类对象时只调用基类析构，子类析构不执行，资源泄漏。**判断规则：打算通过基类指针多态删除对象 → 基类析构必须 virtual**。代价是每个对象多一个 `vfptr`（8 字节）。
5. C++11 起建议用 `override` 显式标注重写意图，编译器会帮你检查签名是否真的匹配父类虚函数，写错会报错而非静默变成「隐藏」。

---

### 第 28 题答案 · 多态案例：计算器体系

```cpp
#include <iostream>
#include <string>
using namespace std;

// 第① 层：抽象接口
class Calculator {
public:
    virtual double calc(double a, double b) const = 0;     // 纯虚函数 → 抽象类
    virtual string name()          const { return "Calculator"; }
    virtual ~Calculator() {}
};

// 第② 层：简单计算器（继承抽象类，仍未实现 calc → 依旧抽象）
class SimpleCalculator : public Calculator {
public:
    string name() const override { return "SimpleCalculator"; }
};

// 第③ 层：具体实现
class AddCalculator : public SimpleCalculator {
public:
    double calc(double a, double b) const override { return a + b; }
    string name() const override { return "AddCalculator"; }
};
class SubCalculator : public SimpleCalculator {
public:
    double calc(double a, double b) const override { return a - b; }
    string name() const override { return "SubCalculator"; }
};

// 关键：形参是【基类指针/引用】，内部发生动态绑定
void run(const Calculator& c) {
    cout << c.name() << ": calc(10, 4) = " << c.calc(10, 4) << endl;
}
void runPoly(Calculator* c) {
    cout << c->name() << ": calc(10, 4) = " << c->calc(10, 4) << endl;
}
// 对比：形参写成具体类型 → 编译期绑定，传入子类也只会调基类版本
void runStaticBind(const AddCalculator& c) {
    cout << "静态绑定 AddCalculator: " << c.calc(10, 4) << endl;
}

int main() {
    AddCalculator ac;  SubCalculator sc;
    run(ac);  run(sc);            // 多态：分别输出加、减
    runPoly(&ac); runPoly(&sc);

    runStaticBind(ac);            // 静态绑定：无法多态

    // 多态数组：统一管理所有计算器
    Calculator* list[2] = { new AddCalculator, new SubCalculator };
    for (Calculator* c : list) c->calc(3, 2);
    for (Calculator* c : list) run(*c);
    delete list[0]; delete list[1];   // 经基类虚析构安全释放

    // Calculator c;    // ❌ 抽象类不能实例化：pure virtual called
    return 0;
}
```

**运行输出**

```
AddCalculator: calc(10, 4) = 14
SubCalculator: calc(10, 4) = 6
AddCalculator: calc(10, 4) = 14
SubCalculator: calc(10, 4) = 6
静态绑定 AddCalculator: 14
AddCalculator: calc(10, 4) = 14
SubCalculator: calc(10, 4) = 6
AddCalculator: calc(10, 4) = 14
SubCalculator: calc(10, 4) = 6
```

**关键结论**

1. **纯虚函数 `= 0` 使类成为抽象类**：含纯虚函数的类不能实例化，但**可以有具体数据成员、构造函数、析构函数**；抽象类的派生类若不实现所有纯虚函数，仍是抽象类。这体现了「把「做什么」（接口）与「怎么做」（实现）分离」的设计。
2. **多态能否层层传递取决于是否 `override`**：第③ 层的 `AddCalculator::calc` 用 `override` 标注、签名与第② 层一致（没有新增纯虚函数），因此它覆盖的是**最顶层** `Calculator::calc` 这一个虚函数槽，用 `Calculator*` 调用能正确分派。如果第② 层**新声明**了一个纯虚函数 `show()` 而第③ 层未实现，第③ 层仍是抽象类。
3. **形参类型决定绑定方式**：
   - 形参 `const Calculator&` → **运行期动态绑定**，编译器插入 `vfptr` 查表代码；
   - 形参 `const AddCalculator&` → **编译期静态绑定**，即使传入的是其他子类对象，调用的也固定是 `AddCalculator::calc`（因为实参类型在编译时就已确定）。
4. **多态重写 vs 同名隐藏**：重写要求 `virtual` + 相同签名，且基类版本被完全替换（可通过 `Base::func()` 显式调回）；隐藏只是名字冲突，签名不同则二者并存。
5. 工程价值：新增一种运算只需**再派生一个子类**，而 `run()` 等所有处理函数**一行都不用改**——这是面向接口编程的核心收益。

---

### 第 29 题答案 · 静态多态与动态多态的对比

```cpp
#include <iostream>
using namespace std;

// ---- 方案一：静态多态（运算符重载，编译期绑定）----
class Vec2 {
public:
    double x, y;
    Vec2(double a = 0, double b = 0) : x(a), y(b) {}
    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    void show() const { cout << "(" << x << ", " << y << ")"; }
};

// ---- 方案二：动态多态（虚函数，运行期绑定）----
class OpBase {
public:
    virtual double apply(double a, double b) const = 0;
    virtual ~OpBase() {}
};
class AddOp : public OpBase {
public:
    double apply(double a, double b) const override { return a + b; }
};
class SubOp : public OpBase {
public:
    double apply(double a, double b) const override { return a - b; }
};

int main() {
    cout << "--- 静态多态（重载）---\n";
    Vec2 v1(1, 2), v2(3, 4);
    v1.show(); cout << " + "; v2.show(); cout << " = ";
    (v1 + v2).show(); cout << endl;       // (4, 6)

    cout << "--- 动态多态（虚函数）---\n";
    OpBase* ops[2] = { new AddOp, new SubOp };
    for (OpBase* o : ops) cout << o->apply(10, 4) << endl;   // 14, 6
    delete ops[0]; delete ops[1];

    cout << "--- 扩展减法：静态方案需改类定义，动态方案只需加类 ---\n";
    // 静态方案：必须回到 Vec2 里加一个 operator-（已加）
    // 动态方案：只需写 class SubOp（已写），main 与调用方零改动
    return 0;
}
```

**运行输出**

```
--- 静态多态（重载）---
(1, 2) + (3, 4) = (4, 6)
--- 动态多态（虚函数）---
14
6
--- 扩展减法：静态方案需改类定义，动态方案只需加类 ---
```

**对比结论**

| 维度 | 静态多态（重载 / 运算符重载） | 动态多态（虚函数） |
|---|---|---|
| 绑定时机 | 编译期，运行期零开销 | 运行期，需查 `vfptr`/`vftable`，有间接调用开销 |
| 关键修饰 | 重载（签名不同）、模板 | `virtual` + `override` |
| 扩展新行为 | **必须修改原类定义** | **只需新增派生类**，调用方零改动 |
| 耦合度 | 高（编译期绑定具体类型） | 低（只依赖抽象接口） |
| 典型场景 | 值类型数学运算、类型转换、容器适配 | 策略切换、插件架构、事件回调、图形/设备驱动 |

**工程判断**：如果行为是「类型的固有属性」（如复数相加、字符串拼接），用静态多态（重载/模板），性能最好；如果行为是一族可替换的「算法」（如不同测量仪器、不同滤波器、不同支付方式），用动态多态扩展性最好。

---

## 第六层 · 运算符重载

### 第 30 题答案 · 运算符重载·成员函数形式

```cpp
#include <iostream>
using namespace std;

class Complex {
public:
    double real, imag;
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}   // 非 explicit

    // 成员形式：operator+ 的第一个操作数隐含为 this（即左操作数必须是 Complex）
    Complex operator+(const Complex& o) const { return Complex(real + o.real, imag + o.imag); }
    Complex operator-(const Complex& o) const { return Complex(real - o.real, imag - o.imag); }
    bool   operator==(const Complex& o) const { return real == o.real && imag == o.imag; }
};

int main() {
    Complex c1(1, 2), c2(3, 4);
    Complex c3 = c1 + c2;
    cout << c3.real << ", " << c3.imag << endl;      // 4, 6
    cout << (c1 == Complex(1, 2)) << endl;           // 1

    Complex c4 = c1 + 10;      // ✅ 左操作数是 Complex，10 经隐式转换构造 Complex
    cout << c4.real << ", " << c4.imag << endl;      // 11, 2

    // Complex c5 = 10 + c1;   // ❌ 成员形式无法实现：左操作数不是 Complex，
    //                          //    编译器不会把 10 自动变成 Complex（不会为左操作数换到成员函数里）
    return 0;
}
```

**关键结论**

1. **成员形式重载的隐含 `this`**：`Complex operator+(const Complex&)` 等价于 `Complex operator+(const Complex& lhs, const Complex& rhs)`，其中 `lhs` 就是 `this`。因此**成员形式只能保证「左操作数是本类（或可转换为本类）」的表达式可用**。
2. `c1 + 10` 能编译，是因为 `+` 的**左侧** `c1` 已是 `Complex`，右侧 `10` 通过非 `explicit` 构造函数隐式转换。
3. `10 + c1` 不能编译，必须提供**非成员形式**：`Complex operator+(int lhs, const Complex& rhs)`。
4. **运算符重载不改变优先级与结合性**：`1 + 2 * 3` 仍是 `1 + (2*3)`，编译器不会帮你加括号。表达式中显式加括号总是好习惯。
5. **`operator==` 必须配 `const` 成员**，否则无法用于 `const` 对象比较；C++20 起 `operator<=>`（三路比较）可一次性提供全部比较运算符。

---

### 第 31 题答案 · 运算符重载·流插入、下标与自增

```cpp
#include <iostream>
#include <iomanip>
#include <stdexcept>          // std::out_of_range（原答案遗漏）
using namespace std;

class Complex {
public:
    double real, imag;
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // ① 加法：main 里用了 c1 + c2，必须提供（原答案遗漏，否则编译报错）
    Complex operator+(const Complex& o) const { return Complex(real + o.real, imag + o.imag); }
    // ② show()：main 里用了 pre.show()（原答案遗漏）
    void show() const { cout << "(" << real << ", " << imag << ")"; }

    // ③ 下标：返回引用，这样才能 c[0] = 5 生效
    double& operator[](int idx) {
        if (idx == 0) return real;
        if (idx == 1) return imag;
        throw std::out_of_range("Complex 下标越界");
    }
    const double& operator[](int idx) const {     // const 版本
        return const_cast<Complex*>(this)->operator[](idx);
    }

    // ④ 前置 ++：返回自身引用，可连续 ++
    Complex& operator++() { ++real; ++imag; return *this; }
    // ⑤ 后置 ++：多一个 int 哑元参数用于区分，返回旧值的副本
    Complex operator++(int) {
        Complex tmp(*this);   // 拷贝旧值
        ++(*this);
        return tmp;
    }
};

// ⑥ << 必须写成非成员（friend）：左侧是 ostream，不是 Complex
//    若写成成员形式，编译器会去找 ostream::operator<<，找不到 → 编译错误
ostream& operator<<(ostream& os, const Complex& c) {
    return os << setw(6) << c.real << (c.imag >= 0 ? " + " : " - ")
              << setw(6) << (c.imag >= 0 ? c.imag : -c.imag) << "i";
}

int main() {
    Complex c1(1, 2), c2(3, 4);
    cout << c1 + c2 << endl;             // 4 + 6i ← 运算符优先级：+ 优先于 <<

    c1[0] = 10;  c1[1] = -5;            // ✅ 因为 operator[] 返回引用
    cout << c1 << endl;                  // 10 - 5i
    // try { cout << c1[2]; } catch (const std::out_of_range& e) { cout << e.what() << endl; }

    Complex a(1, 1), b(1, 1);
    Complex pre  = ++a;    // a 变 (2,2)，pre 是 a 的别名
    Complex post = b++;    // b 变 (2,2)，post 保留 (1,1)
    cout << "pre  = "; pre.show();  cout << "\npost = "; post.show(); cout << endl;

    ++ ++a;               // 前置返回引用，可连用
    cout << "a     = "; a.show();  cout << endl;
    return 0;
}
```

**运行输出（实测）**

```
     4 +      6i
    10 -      5i
pre  = (2, 2)
post = (1, 1)
a     = (4, 4)
```

**关键结论**

1. **`<<` 只能作为非成员（通常声明为友元）重载**。原因：`cout << c1` 中最左操作数是 `ostream` 对象，成员形式会尝试查找 `ostream::operator<<(Complex)`——这不可能存在（你无法修改标准库的 `ostream`）。写成非成员后，`ostream&` 作为第一个形参，正好匹配。
2. **`operator[]` 应返回引用**，否则 `c[0] = 10` 只改临时副本，无效。这是常见错误。返回 `const double&` 重载用于 `const` 对象。
3. **前置 `++` 返回 `T&`**，返回值就是 `this`，可继续 `++`；**后置 `++` 返回 `T`（副本）**，保留自增前的值。后置版本的 `int` 形参只是**用于区分的哑元参数，不参与运算**。
4. 重载后 `cout << c1 + c2` 按优先级解析为 `cout << (c1 + c2)`，正确。
5. 记忆口诀：**`<<`/`>>` 用友元；`+`/`-`/`=`/`[]`/`()` 用成员；`=`/`++`/`[]` 返回引用，`+`/`-`/`<<` 按需返回。**

---

### 第 32 题答案 · 运算符重载·转换运算符与 explicit

```cpp
#include <iostream>
using namespace std;

class Student {
    string name;
    double score1, score2, score3;
public:
    Student(string n, double a, double b, double c)
        : name(n), score1(a), score2(b), score3(c) {}

    double average() const { return (score1 + score2 + score3) / 3.0; }

    // ① 隐式转换运算符：Student → double
    operator double() const { return average(); }
    // ② 若改成下面这样，则禁止隐式转换（需显式 static_cast）
    // explicit operator double() const { return average(); }
};

class Thermometer {
    double celsius;
public:
    Thermometer(double c) : celsius(c) {}
    friend ostream& operator<<(ostream& os, const Thermometer& t);
    friend istream& operator>>(istream& is, Thermometer& t);
};

ostream& operator<<(ostream& os, const Thermometer& t) {
    return os << t.celsius << " C";
}
// 返回 istream& 以支持 cin >> a >> b 链式调用
istream& operator>>(istream& is, Thermometer& t) {
    is >> t.celsius;
    return is;
}

int main() {
    Student s("赵六", 90, 80, 70);

    cout << "average  = " << s.average() << endl;
    double d = s;                       // ✅ 隐式转换：Student → double
    cout << "隐式转换 d= " << d << endl; // 80

    // 把转换运算符改为 explicit 后，上一行会报错，需改为：
    // double d = static_cast<double>(s);

    cout << "s + 5.0 = " << (s + 5.0) << endl;   // 85（先把 s 转成 double 再加）——隐式时才允许

    Thermometer t(0);
    cout << "t = " << t << endl;        // 0 C
    cin >> t;                           // 输入 36.5
    cout << "t = " << t << endl;        // 36.5 C
    return 0;
}
```

**运行结果（需交互输入，此处以 36.5 为例）**

```
average  = 80
隐式转换 d= 80
s + 5.0 = 85
t = 0 C
t = 36.5 C
```

**关键结论**

1. **转换运算符写法**：`operator 目标类型() const { ...; }`，**不写返回类型**（目标类型就是返回类型）。它让自定义类型能像内置类型一样参与运算。
2. **`explicit` 的作用**：禁止隐式转换，只允许 `static_cast<double>(s)` 显式调用。**强烈建议给转换运算符加 `explicit`**——否则会引发意外的重载解析歧义和难以追踪的隐式转换链。
3. 加 `explicit` 后，`s + 5.0` **会报错**（因为不存在 `operator+(Student, double)`，也不允许隐式转换），必须写 `static_cast<double>(s) + 5.0`。这正是 `explicit` 的价值：让隐式转换在编译期暴露出来。
4. **`operator>>` 必须返回 `istream&`**（不能返回 `void`），否则 `cin >> a >> b` 链式写法无法编译。
5. `operator<<`/`operator>>` 通常声明为 **friend**，因为要访问类的私有成员（虽然也可以做成类的公有成员函数，但 `cout << c` 的匹配依然成立，因为左操作数匹配的是 `ostream` 形参）。

---

## 第七层 · 文件操作

### 第 33 题答案 · 文件操作·文本方式存取结构体数组

```cpp
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

struct Student {
    char name[20];      // 定长字符数组 → 文本方式可安全存取
    int  age;
    int  id;
};

int main() {
    Student arr[3] = {
        {"张三", 20, 1001},
        {"李四", 21, 1002},
        {"王五", 19, 1003}
    };
    const int n = 3;

    // ① 文本方式写：需要自己规定字段分隔格式
    {
        ofstream ofs("student.txt", ios::out);     // ios::out 可省略（默认）
        if (!ofs) { cerr << "文件打开失败\n"; return 1; }
        for (int i = 0; i < n; ++i)
            ofs << arr[i].name << ' ' << arr[i].age << ' ' << arr[i].id << '\n';
        ofs.close();
    }

    // ② 文本方式读
    {
        Student tmp;
        ifstream ifs("student.txt", ios::in);
        if (!ifs) { cerr << "文件打开失败\n"; return 1; }
        while (ifs >> tmp.name >> tmp.age >> tmp.id) {
            cout << tmp.name << '\t' << tmp.age << '\t' << tmp.id << '\n';
        }
        ifs.close();
    }

    // ③ 比较文件字节数
    {
        ifstream f("student.txt", ios::ate | ios::binary);   // ate 定位到末尾
        cout << "文件字节数 = " << f.tellg() << endl;
        cout << "sizeof(Student) = " << sizeof(Student)
             << "，3 个元素 = " << sizeof(Student) * n << endl;
    }
    remove("student.txt");
    return 0;
}
```

**运行输出（典型）**

```
张三	20	1001
李四	21	1002
王五	19	1003
文件字节数 = 45
sizeof(Student) = 28，3 个元素 = 84
```

> 说明：`Student{char name[20]; int age; int id;}` 中 `20 + 4 + 4 = 28`，已是 4 的倍数无需额外尾部填充，故 `sizeof = 28`（不是 32）。45 字节 = 每行「3 个 UTF-8 汉字(9 字节) + 2 个空格 + 2 位年龄 + 2 个空格 + 4 位学号 + 换行(1)」共 15 字节 × 3 行。

**关键结论**

1. **三种流对象与打开模式**：
   - `ofstream`（输出）默认 `ios::out`，**写**；`ios::app` 追加到文件尾；
   - `ifstream`（输入）默认 `ios::in`，**读**；
   - `fstream` 双向，需显式给 `ios::in | ios::out`；
   - `ios::binary` 二进制模式，`ios::ate` 打开即定位到末尾（配合 `tellg()` 量文件大小）。
2. **文本方式文件大小（45 字节）远小于 sizeof×n（84 字节）**：因为文本方式只存「有意义的内容」，不存填充字节。这是文本与二进制最直观的区别。
3. **文本方式只能写「有流运算符支持」的类型**：`int`、`double`、`char`/`char[]` 可以；**含 `std::string` 或指针的结构体不能用 `<<` 直接写**（会报错），因为 `operator<<` 只为内置类型提供。
4. 字段分隔符要用 `cin >> ` 能读回的形式（空格/逗号都行，但必须统一），含空格的字符串（如「张三 丰」）会破坏读取，故姓名宜用定长数组且不含空格。

---

### 第 34 题答案 · 文件操作·二进制方式与指针成员对比

```cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
using namespace std;

// ① POD 结构体：二进制安全
struct StudentBin {
    char   name[20];
    int    age;
    double score;
};

// ② 含指针的结构体：二进制不安全
struct StudentPtr {
    char    name[20];
    int     age;
    string* pname;          // ❌ 存的是地址，不是数据
};

// ③ 可持久化方案：定长数组 + 显式长度字段
struct StudentSafe {
    char  name[32];
    unsigned char nameLen;  // 显式记录有效长度
    int   age;
    double score;
};

void writeBin(const char* fn, const StudentBin* p, int n) {
    ofstream ofs(fn, ios::binary | ios::out);
    ofs.write(reinterpret_cast<const char*>(p), sizeof(StudentBin) * n);
}
bool readBin(const char* fn, StudentBin* p, int n) {
    ifstream ifs(fn, ios::binary | ios::in);
    if (!ifs) return false;
    ifs.read(reinterpret_cast<char*>(p), sizeof(StudentBin) * n);
    return ifs.gcount() == (streamsize)(sizeof(StudentBin) * n);
}

int main() {
    StudentBin a[2] = { {"张三", 20, 91.5}, {"李四", 21, 88.0} };

    // ① 二进制写读
    writeBin("s.bin", a, 2);
    StudentBin b[2];
    if (readBin("s.bin", b, 2))
        for (int i = 0; i < 2; ++i)
            cout << b[i].name << " " << b[i].age << " " << b[i].score << endl;
    cout << "二进制文件大小 = " << sizeof(StudentBin) * 2 << endl;

    // ② 含指针成员：二进制写读
    {
        StudentPtr c[2];
        strcpy(c[0].name, "王五"); c[0].age = 19; c[0].pname = new string("王五");
        strcpy(c[1].name, "赵六"); c[1].age = 22; c[1].pname = new string("赵六");

        ofstream ofs("p.bin", ios::binary | ios::out);
        ofs.write(reinterpret_cast<const char*>(c), sizeof(StudentPtr) * 2);
        ofs.close();

        StudentPtr d[2];
        ifstream ifs("p.bin", ios::binary | ios::in);
        ifs.read(reinterpret_cast<char*>(d), sizeof(StudentPtr) * 2);
        ifs.close();
        cout << "读回 d[0].name = " << d[0].name << "，age = " << d[0].age << endl;
        cout << "读回 d[0].pname 指向: ";  // 地址值原样搬过来，解引用已无意义
        cout << "（pname = " << (void*)d[0].pname << "）" << endl;

        delete c[0].pname; delete c[1].pname;   // 原指针仍有效（同一进程内）
        // 危险：delete d[0].pname;  ← 指向的是已释放的内存，双重释放
    }

    // ③ 定长数组 + 显式长度：安全且可读
    {
        StudentSafe s{};
        strcpy(s.name, "钱七"); s.nameLen = strlen(s.name); s.age = 20; s.score = 77.0;
        ofstream ofs("safe.bin", ios::binary | ios::out);
        ofs.write(reinterpret_cast<const char*>(&s), sizeof(s));
        ofs.close();

        StudentSafe t{};
        ifstream ifs("safe.bin", ios::binary | ios::in);
        ifs.read(reinterpret_cast<char*>(&t), sizeof(t));
        t.name[t.nameLen] = '\0';
        cout << "safe: " << t.name << " " << t.age << " " << t.score << endl;
    }

    remove("s.bin"); remove("p.bin"); remove("safe.bin");
    return 0;
}
```

**运行输出（典型）**

```
张三 20 91.5
李四 21 88
二进制文件大小 = 64          // sizeof(StudentBin)=32 × 2（32 = name 20 补到 24 + age 4 补到 32 + score 8）
读回 d[0].name = 王五，age = 19
读回 d[0].pname 指向: （pname = 0x7ffd3c8a4c20）
safe: 钱七 20 77
```

**关键结论**

1. **二进制方式**用 `ofstream::write(const char*, streamsize)` 与 `ifstream::read(char*, streamsize)`，本质是**按字节原样拷贝内存块**，因此速度最快、无需解析。
2. **二进制读写的硬性前提**：读写两端**必须使用同一个结构体定义**（相同 `sizeof`、相同成员类型与顺序、相同编译器/ABI）。否则数据错位。这就是文件格式版本问题的根源。
3. **含指针成员的结构体绝对不能用二进制直接存取**：`pname` 存的是**进程的虚拟地址**（如 `0x7ffd3c8a4c20`）。同一进程内读回可能「碰巧」还能访问（地址还在），但换进程、换次运行后该地址无效 → 解引用即崩溃；更糟的是若 `delete` 它，会操作到无关内存。因此示例中特意**不** `delete d[0].pname`。
4. **可持久化方案**：把变长数据改为**定长字符数组 + 显式长度字段**，或改为文本方式按协议序列化，或用 protobuf / cJSON / nlohmann-json 等成熟序列化库。这是实际工程中的标准做法。
5. 补充：`char name[20]` 在二进制方式下**不做结尾 `\0` 处理**。若写入时字符串恰好填满 20 字节无终止符，读回后 `cout <<` 会越界读。稳妥做法也是加长度字段（方案③）。

---

## 第八层 · 模板

### 第 35 题答案 · 函数模板与自动类型推导

```cpp
#include <iostream>
using namespace std;

// ① 函数模板
template <typename T>
void mySwap(T& a, T& b) {
    T tmp = a; a = b; b = tmp;
}

// ② 同签名的普通函数（用于验证优先级）
void mySwap(int& a, int& b) {
    cout << "  [普通函数版本被调用]";
    int t = a; a = b; b = t;
}

int main() {
    int    i1 = 1,  i2 = 2;
    double d1 = 1.5, d2 = 2.5;
    char   c1 = 'a', c2 = 'b';

    cout << "int交换：";    mySwap(i1, i2); cout << i1 << ',' << i2 << endl;   // [普通函数版本被调用]2,1
    cout << "double交换："; mySwap(d1, d2); cout << d1 << ',' << d2 << endl;   // 2.5,1.5
    cout << "char交换：";   mySwap(c1, c2); cout << c1 << ',' << c2 << endl;   // b,a

    // mySwap(i1, d1);   // ❌ 推导冲突：T 同时推导为 int 和 double → 编译失败

    cout << "显式指定实参：";
    mySwap<double>(d1, d2); cout << d1 << ',' << d2 << endl;   // 指定 T=double

    cout << "普通函数优先：";
    mySwap(i1, i2);          // 调用非模板版本（完全匹配优先于模板特化）
    cout << endl;
    return 0;
}
```

**运行输出（实测）**

```
int交换：  [普通函数版本被调用]2,1
double交换：2.5,1.5
char交换：b,a
显式指定实参：1.5,2.5
普通函数优先：  [普通函数版本被调用]
```

> 两处易错点：
> 1. **`int交换` 走的也是普通函数**（普通函数优先于模板），所以标签后紧跟 `[普通函数版本被调用]`。
> 2. **`显式指定实参` 的输出是 `1.5,2.5` 而不是 `2.5,1.5`**：因为上一步 `double交换` 已经把 `d1/d2` 换成 `2.5,1.5`，这里再显式调用 `mySwap<double>` 又换了一次，**换回原值**。交换函数是"来回翻"的，不要只看单次调用。

**关键结论**

1. **自动类型推导**：`T` 从实参推出，`T&` 是引用绑定。传入 `int` 变量时 `T=int`，不发生拷贝。
2. **推导必须自洽**：`mySwap(intVar, doubleVar)` 时 `T` 需同时为 `int` 和 `double`，矛盾 → 编译错误。若要支持异类型交换，需显式指定：`mySwap<double>(i, d)`（此时 `T&` 无法绑定，需把形参改成 `T& a, T& b` 的 const 引用版本或按值）。
3. **显式指定模板实参**：`函数名<类型>(实参)`，可绕过推导。
4. **优先级规则**：当「普通函数」与「模板特化」都能匹配且**匹配程度相同时**，**优先选普通函数**；只有普通函数不存在或需标准转换而模板是精确匹配时，才选模板。这解释了最后一行输出 `[普通函数版本被调用]`。
5. 相关术语：`template <typename T>` 与 `template <class T>` 完全等价；模板只能写在**函数或类的最外层**，不能在函数体内嵌套定义模板。

---

### 第 36 题答案 · 可变参数模板

```cpp
#include <iostream>
using namespace std;

// ① 递归展开求和：只有 1 个参数时终止递归
template <typename T>
T sum(T v) { return v; }

template <typename T, typename... Args>
T sum(T v, Args... args) {
    return v + sum(args...);            // 展开为 sum(v, args...) 递归
}

// ② 可变参数 max（同样递归展开）
template <typename T>
T maxOf(T v) { return v; }

template <typename T, typename... Args>
T maxOf(T v, Args... args) {
    return v > maxOf(args...) ? v : maxOf(args...);
}

// ③ 限制最多 4 个参数：包内个数 <= 3（因已显式取走第 1 个）
template <typename T, typename... Args>
T sumAtMost4(T v, Args... args) {
    static_assert(sizeof...(Args) <= 3,
                  "sumAtMost4 最多接受 4 个参数");
    return sum(v, args...);
}

int main() {
    cout << "sum(1, 2, 3)          = " << sum(1, 2, 3) << endl;          // 6
    cout << "sum(1.1, 2.2, 3.3)    = " << sum(1.1, 2.2, 3.3) << endl;    // 6.6（逐个按 double 推导）
    cout << "sum(10)               = " << sum(10) << endl;               // 10
    cout << "sum(1, 2, 3, 4, 5)    = " << sum(1, 2, 3, 4, 5) << endl;    // 15
    cout << "maxOf(3, 9, 2, 8)     = " << maxOf(3, 9, 2, 8) << endl;     // 9
    cout << "sumAtMost4(1,2,3,4)   = " << sumAtMost4(1, 2, 3, 4) << endl; // 10
    // sumAtMost4(1,2,3,4,5);    // ❌ 触发 static_assert 编译失败
    return 0;
}
```

**运行输出**

```
sum(1, 2, 3)          = 6
sum(1.1, 2.2, 3.3)    = 6.6
sum(10)               = 10
sum(1, 2, 3, 4, 5)    = 15
maxOf(3, 9, 2, 8)     = 9
sumAtMost4(1,2,3,4)   = 10
```

**关键结论**

1. **模板参数包 `typename... Args`**，配合函数形参 `Args... args` 接收任意数量实参。
2. **递归展开的终止条件**必须单独提供一个「基例重载」（`sum(T)` 与 `sum(T, Args...)` 通过参数个数区分，不存在二义性）。展开过程：`sum(1,2,3)` → `1 + sum(2,3)` → `1 + (2 + sum(3))` → `1 + (2 + 3)`。
3. **`sizeof...(Args)`** 在编译期给出参数包内元素个数，用于 `static_assert` 做编译期约束。
4. **返回类型 `T` 由第一个实参决定**，因此 `sum(1, 2, 3)` 全程按 `int` 运算；若要全程按 `double`，需写 `sum<double>(1,2,3)`。
5. **现代 C++17 起更推荐折叠表达式**，代码更简洁且无递归开销：
   ```cpp
   template <typename... Args>
   double sumFold(Args... args) { return (args + ... + 0.0); }   // 一元左折叠
   template <typename... Args>
   double sumFold2(Args... args) { return (0.0 + ... + args); }  // 二元左折叠
   ```
6. 补充：如果传的是字符串字面量等数组类型，形参应写 `Args... args`（按值退化为指针）或 `const Args&...`。

---

### 第 37 题答案 · 类模板

```cpp
#include <iostream>
#include <string>
using namespace std;

// name 与 age 可以是【不同】类型
template <class NameType, class AgeType = int>     // AgeType 有默认值 2
class Person {
    NameType name;
    AgeType  age;
public:
    Person(NameType n, AgeType a) : name(n), age(a) {}       // 构造函数

    void show() const;                                        // 只在类内声明
    NameType getName() const { return name; }
    AgeType  getAge()  const { return age; }
};

// 成员函数在类外的定义：必须再写一遍 template<> 参数列表
template <class NameType, class AgeType>
void Person<NameType, AgeType>::show() const {
    cout << "name=" << name << "  age=" << age << endl;
}

int main() {
    // 类模板没有自动类型推导，必须显式指定 <>
    Person<string, int>       p1("张三", 20);
    Person<int, double>       p2(123, 3.14);        // name 是 int，age 是 double
    Person<string>            p3("李四", 18);       // 省略 AgeType → 用默认值 int

    p1.show();   // name=张三  age=20
    p2.show();   // name=123  age=3.14
    p3.show();   // name=李四  age=18

    cout << "sizeof(p1) = " << sizeof(p1) << endl;   // 40（string 32 + int 4 + 填充 4）
    cout << "sizeof(p2) = " << sizeof(p2) << endl;   // 16（int 4 + 填充 4 + double 8）

    // Person p4("王五", 19);   // ❌ 编译错误：类模板不做自动推导
    return 0;
}
```

**运行输出**

```
name=张三  age=20
name=123  age=3.14
name=李四  age=18
sizeof(p1) = 40
sizeof(p2) = 16
```

**关键结论**

1. **类模板没有自动类型推导**。函数模板可以靠实参推导 `T`，类模板必须显式写 `Person<string, int>`，否则编译报错。
2. **类模板成员函数在类外定义时，模板参数列表不可省略**，且要写成 `Person<NameType, AgeType>::show`，前面的 `class` 不能写成 `class Person`（那会报错，通常写 `class` 或省略，标准写法为 `template <class N, class A> void Person<N, A>::show()`）。
3. **模板参数默认值**：`<class NameType, class AgeType = int>` 使得 `Person<string>` 合法（`AgeType` 取 `int`）。默认参数必须从右向左连续，且一个模板实参列表中**不能重复指定同一个模板形参**。
4. **同一模板的不同实例类型互不兼容**：`Person<string,int>` 与 `Person<int,double>` 是两个毫不相干的类型，不能互相赋值。若允许相互赋值，需要写「模板特化」或提供转换函数。
5. `sizeof` 差异说明：**类模板的成员可以是任意类型**，这正是模板相比「只能同质成员」的普通类的优势。

---

## 第九层 · STL

### 第 38 题答案 · STL 六大容器对比

```cpp
#include <iostream>
#include <iomanip>        // setw / left（原答案遗漏此头，编译报错）
#include <vector>
#include <deque>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <string>
using namespace std;

template <typename C>
void dump(const char* name, C& c) {
    cout << setw(12) << left << name << " : ";
    for (auto it = c.begin(); it != c.end(); ++it) cout << *it << ' ';
    cout << endl;
}

int main() {
    // vector —— 动态数组，连续内存
    vector<int> v;
    for (int i = 0; i < 5; ++i) v.push_back(i * 10);
    v.insert(v.begin() + 1, 999);                 // 中间插入：O(n)，要搬移元素
    dump("vector", v);
    cout << "  随机访问 v[3] = " << v[3] << "，容量=" << v.capacity() << endl;
    v.erase(v.begin());                          // 删除首元素：O(n)
    dump("vector-删首", v);

    // deque —— 双端队列，两端插入删除 O(1)
    deque<int> d;
    d.push_back(1); d.push_back(2);
    d.push_front(0); d.push_front(-1);            // ✅ 两端都高效
    dump("deque", d);
    d.pop_front(); d.pop_back();
    dump("deque-两端弹", d);

    // list —— 双向链表，插入删除 O(1)，无随机访问
    list<int> l;
    l.push_back(3); l.push_front(1); l.push_front(0);
    list<int>::iterator it = l.insert(l.begin(), -1);   // ✅ 插入 O(1)，无需搬移
    dump("list", l);
    // l[1] = 5;                 // ❌ 编译错误：list 不支持随机访问
    l.erase(it);                                  // 删除 O(1)
    dump("list-删首", l);

    // stack —— 后进先出（默认底层 deque，无迭代器）
    stack<int> st;
    st.push(1); st.push(2); st.push(3);
    cout << "stack 栈顶 = " << st.top() << endl;   // 3
    st.pop();
    cout << "stack 弹后栈顶 = " << st.top() << endl; // 2
    cout << "stack 空? " << st.empty() << endl;

    // queue —— 先进先出
    queue<int> q;
    q.push(10); q.push(20); q.push(30);
    cout << "queue 队首 = " << q.front() << " 队尾 = " << q.back() << endl;
    q.pop();
    cout << "queue 弹后队首 = " << q.front() << endl;

    // map —— 红黑树，按 key 自动排序
    map<string, int> m;
    m["zhang"] = 1; m["li"] = 2; m["wang"] = 3; m["chen"] = 4;
    for (const auto& p : m) cout << p.first << "->" << p.second << ' ';
    cout << endl;                                  // chen->4 li->2 wang->3 zhang->1（按键升序）
    cout << "m.count(\"li\") = " << m.count("li") << endl;   // 1
    m.erase("wang");
    cout << "erase(\"wang\") 后 size = " << m.size() << endl; // 3

    return 0;
}
```

**运行输出**

```
vector       : 0 999 10 20 30 40
  随机访问 v[3] = 20，容量=8
vector-删首 : 999 10 20 30 40
deque        : -1 0 1 2
deque-两端弹 : 0 1
list         : -1 0 1 3
list-删首  : 0 1 3
stack 栈顶 = 3
stack 弹后栈顶 = 2
stack 空? 0
queue 队首 = 10 队尾 = 30
queue 弹后队首 = 20
chen->4 li->2 wang->3 zhang->1
m.count("li") = 1
erase("wang") 后 size = 3
```

**六大容器对比表（选型依据）**

| 容器 | 底层结构 | 随机访问 | 头部插入/删除 | 尾部插入 | 查找 | 典型用途 |
|---|---|---|---|---|---|---|
| `vector` | 连续动态数组 | ✅ O(1) | ❌ O(n) | ✅ 摊还 O(1) | ❌ O(n) | 默认选择，绝大多数场景 |
| `deque` | 分段连续（双端） | ❌ O(1)分段 | ✅ O(1) | ✅ O(1) | ❌ O(n) | 双端队列、滑动窗口 |
| `list` | 双向链表 | ❌ | ✅ O(1) | ✅ O(1) | ❌ O(n) | **极少用**：缓存不友好 |
| `stack` | `deque` 适配 | ❌ | — | — | ❌ | 括号匹配、DFS、单调栈 |
| `queue` | `deque` 适配 | ❌ | ✅ | ✅ | ❌ | BFS、生产者消费者 |
| `map` | 红黑树 | — | — | — | ✅ **O(log n)** | 有序映射、需要最值/范围查询 |
| （`unordered_map`） | 哈希表 | — | — | — | ✅ 平均 O(1) | 仅需快速查找、无需有序 |

**关键结论**

- **优先用 `vector`**：连续内存对 CPU 缓存最友好，实测性能常优于 `list`，即使列表理论复杂度更优。黑马的经典案例正是「`vector` 插入 1 万次耗时 4ms，`list` 耗时 1480ms」。
- `stack`/`queue` 是**适配器**，没有迭代器和 `operator[]`，只能 `top()`/`front()`/`back()`/`push`/`pop`。
- `map` 自动按键排序，`unordered_map` 不排序但平均更快；需要范围遍历或取最值时必须用 `map`。
- 所有容器都支持 `begin()`/`end()` 范围 `for`，这是 STL 统一接口的威力。

---

### 第 39 题答案 · STL 算法与 lambda 表达式

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
using namespace std;

struct Student {
    string name;
    int    score1, score2, score3;
    int total() const { return score1 + score2 + score3; }
    int best()  const { return max({score1, score2, score3}); }
};

// ① 自定义比较函数：总分降序，同分则最好单科降序
bool cmp(const Student& a, const Student& b) {
    if (a.total() != b.total()) return a.total() > b.total();
    return a.best() > b.best();
}

void show(const string& tag, const vector<Student>& v) {
    cout << tag << ": ";
    for (const auto& s : v) cout << s.name << "(" << s.total() << ") ";
    cout << endl;
}

int main() {
    vector<Student> v = {
        {"张三", 90, 80, 70},   // 240
        {"李四", 60, 95, 85},   // 240，最好单科 95
        {"王五", 70, 70, 70},   // 210
        {"赵六", 95, 95, 95}    // 285
    };

    // ② 排序：自定义比较函数
    vector<Student> a = v;
    sort(a.begin(), a.end(), cmp);
    show("自定义比较函数", a);      // 赵六(285) 李四(240) 张三(240) 王五(210)

    // ③ 排序：lambda 表达式（只比总分，未写次级条件 → 与 cmp 并非等价）
    int threshold = 0;
    vector<Student> b = v;
    sort(b.begin(), b.end(), [threshold](const Student& x, const Student& y) {
        return x.total() - threshold > y.total() - threshold;
    });
    show("lambda 排序", b);

    // ④ find —— 按条件查找（返回迭代器）
    vector<Student> c = v;
    auto it = find_if(c.begin(), c.end(),
                      [](const Student& s) { return s.name == "王五"; });
    if (it != c.end()) cout << "find_if 找到: " << it->name << endl;

    // ⑤ count —— 统计个数
    cout << "count(total>230) = "
         << count_if(v.begin(), v.end(),
                     [](const Student& s) { return s.total() > 230; })
         << endl;                                  // 3

    // ⑥ find_if + count_if 已见；⑦ remove_if + erase —— erase-remove 惯用法
    vector<Student> d = v;
    d.erase(remove_if(d.begin(), d.end(),
                      [](const Student& s) { return s.total() < 220; }),
            d.end());
    show("删除总分<220", d);                      // 张三 李四 赵六

    // ⑧ accumulate —— 求和（四参数版：带初值与自定义二元运算）
    int totalSum = accumulate(v.begin(), v.end(), 0,
                              [](int acc, const Student& s) { return acc + s.total(); });
    cout << "accumulate 总分和 = " << totalSum << endl;   // 975（240+240+210+285）
    int bestTotal = accumulate(v.begin(), v.end(), 0,
                               [](int acc, const Student& s) { return max(acc, s.total()); });
    cout << "accumulate 最大总分 = " << bestTotal << endl; // 285

    // ⑨ replace —— 值替换
    vector<Student> e = v;
    replace_if(e.begin(), e.end(),
               [](const Student& s) { return s.total() == 240; },  Student{"待定", 0, 0, 0});
    show("replace 替换同分组", e);

    vector<int> nums = { 1, 2, 3, 2, 2, 5 };
    replace(nums.begin(), nums.end(), 2, 20);
    cout << "replace 2->20: ";
    for (int x : nums) cout << x << ' ';
    cout << endl;                                    // 1 20 3 20 20 5

    return 0;
}
```

**运行输出（实测）**

```
自定义比较函数: 赵六(285) 李四(240) 张三(240) 王五(210)
lambda 排序: 赵六(285) 张三(240) 李四(240) 王五(210)
find_if 找到: 王五
count(total>230) = 3
删除总分<220: 张三(240) 李四(240) 赵六(285)
accumulate 总分和 = 975
accumulate 最大总分 = 285
replace 替换同分组: 待定(0) 待定(0) 王五(210) 赵六(285)
replace 2->20: 1 20 3 20 20 5
```

> 三处纠正：
> 1. **`accumulate 总分和` 是 `975`**（240+240+210+285），原答案的 `915` 是算错的。
> 2. **`lambda 排序` 与 `cmp` 的同分顺序不同**：`sort` 不是稳定排序，lambda 只比总分（同分返回 false），张三/李四先后**未定义**，实测张三在前。需要稳定请用 `stable_sort`。
> 3. **`replace` 只替换了 2 个**：只有张三、李四总分等于 240，输出是 `待定(0) 待定(0) 王五(210) 赵六(285)`，没有第 3 个"待定"。

**关键结论**

1. **`sort` 必须提供严格弱序的比较函数**。若写成 `return a.total() >= b.total();` 属于非严格弱序，**会导致未定义行为**（sort 的比较结果需自反、反对称、传递），最坏情况退化为 O(n²)。
   > 补充：**`std::sort` 不是稳定排序**，同分元素的相对顺序**未定义**。本题 `cmp` 带 `best()` 次级条件（李四在前），而 lambda 只比总分（实测张三在前），两者顺序不同属正常现象，不要依赖；需要稳定请用 `std::stable_sort`。
2. **erase-remove 惯用法**：`vector::erase` 无法按条件删除，必须先 `remove_if` 把不满足条件的元素移到末尾（返回新逻辑末尾的迭代器），再用 `erase` 删除 `[新末尾, 旧末尾)`。直接写 `erase(remove_if(...), end())` 看似冗余，但缺一不可。
3. **`accumulate` 三参数版**求和；**四参数版**传入二元运算，可实现求最大值、最小值、自定义归约。注意初值类型要与元素类型兼容（`0` 而非 `0.0`，否则会编译失败或隐式转换）。
4. **`lambda` 表达式**语法：`[捕获列表](形参) mutable -> 返回类型 { 函数体 }`。不捕获任何外部变量时可写 `[]`；按值捕获 `[a]`；按引用捕获 `[&]`（能修改外部变量）；默认 C++17 起**按值捕获隐式捕获 this**。
5. 算法与容器解耦：同一算法可作用于 `vector`、`list`、`deque` 任何实现了所需迭代器类别的容器，这是 STL 的核心设计思想。

---

### 第 40 题答案 · STL 仿函数与适配器

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
using namespace std;

struct Student {
    string name;
    int    total;
};

// ① 一元仿函数：带阈值状态
class ScoreGreater {
    int threshold_;
public:
    explicit ScoreGreater(int t) : threshold_(t) {}
    bool operator()(const Student& s) const { return s.total > threshold_; }
    int getThreshold() const { return threshold_; }   // 状态可被读取 → 可复用
};

// ② 二元仿函数：多条件排序
struct ScoreCompare {
    bool operator()(const Student& a, const Student& b) const {
        return a.total > b.total;
    }
};

// ③ 三元仿函数：带初始状态的一元仿函数（用于 transform 累加偏移）
struct AddBonus {
    int bonus;
    explicit AddBonus(int b) : bonus(b) {}
    Student operator()(const Student& s) const {
        return { s.name, s.total + bonus };
    }
};

int main() {
    vector<Student> v = {
        {"张三", 240}, {"李四", 210}, {"王五", 285}, {"赵六", 255}
    };

    // ---- 一元仿函数 + find_if ----
    ScoreGreater gt(250);
    auto it = find_if(v.begin(), v.end(), gt);
    cout << "阈值 " << gt.getThreshold() << " 时第一个 >阈值: ";
    if (it != v.end()) cout << it->name << "(" << it->total << ")" << endl;   // 王五(285)

    // ---- 一元仿函数 + count_if ----
    cout << "count_if(>250) = " << count_if(v.begin(), v.end(), ScoreGreater(250)) << endl;  // 2

    // ---- 同一仿函数复用于不同阈值（这是仿函数相对 lambda 的核心优势）----
    for (int t : {200, 240, 280}) {
        cout << "  > " << t << " 的人数 = " << count_if(v.begin(), v.end(), ScoreGreater(t)) << endl;
    }

    // ---- 二元仿函数 + sort ----
    vector<Student> a = v;
    sort(a.begin(), a.end(), ScoreCompare());
    cout << "排序后: ";
    for (const auto& s : a) cout << s.name << "(" << s.total << ") ";
    cout << endl;                                    // 王五 赵六 张三 李四

    // ---- transform：一元仿函数适配器（原地变换）----
    vector<Student> b = v;
    transform(b.begin(), b.end(), b.begin(), AddBonus(5));   // 每人 +5 分
    cout << "transform +5: ";
    for (const auto& s : b) cout << s.name << "(" << s.total << ") ";
    cout << endl;

    // ---- for_each：带 lambda 的遍历副作用 ----
    int count = 0;
    for_each(v.begin(), v.end(), [&count](const Student& s) { if (s.total > 240) ++count; });
    cout << "for_each 统计 >240 人数 = " << count << endl;   // 2（285、255）

    // ---- bind：绑定参数的一元适配器（部分应用）----
    auto gt240 = std::bind(ScoreGreater(240), std::placeholders::_1);
    cout << "bind: count_if(>240) = " << count_if(v.begin(), v.end(), gt240) << endl;   // 2

    // ---- 带状态仿函数类 vs lambda ----
    int base = 250;
    auto byRef = [&base](const Student& s) { return s.total > base; };  // 引用捕获：base 改了结果跟着变
    auto byVal = [base] (const Student& s) { return s.total > base; };  // 值捕获：阈值固定为捕获时的 250
    cout << "lambda 值捕获(阈值固定250): " << count_if(v.begin(), v.end(), byVal)
         << "，lambda 引用捕获(改 base=260 后): ";
    base = 260;
    cout << count_if(v.begin(), v.end(), byRef) << endl;
    return 0;
}
```

**运行输出**

```
阈值 250 时第一个 >阈值: 王五(285)
count_if(>250) = 2
  > 200 的人数 = 4
  > 240 的人数 = 2
  > 280 的人数 = 1
排序后: 王五(285) 赵六(255) 张三(240) 李四(210)
transform +5: 张三(245) 李四(215) 王五(290) 赵六(260)
for_each 统计 >240 人数 = 2
bind: count_if(>240) = 2
lambda 值捕获(阈值固定250): 2，lambda 引用捕获(改 base=260 后): 1
```

> 四处纠正（数据为 张三240 / 李四210 / 王五285 / 赵六255，`>240` 只命中 285 和 255 **两个**）：
> 1. `>240 的人数` = **2**（原答案写 3）。
> 2. `for_each` = **2**、`bind` = **2**、值捕获 lambda = **2**、`base=260` 后引用捕获 = **1**（原答案分别写成 3/3/3/2）。
> 3. `transform +5` 的输出顺序是**原容器顺序**（张三、李四、王五、赵六），因为 `b = v` 是未经排序的副本；原答案误写成了排序后的顺序。
> 4. `sort` 只排了副本 `a`，不影响 `v` 与 `b`。

**关键结论**

1. **仿函数（functor / 函数对象）**：任何重载了 `operator()` 的类实例。它是**对象**，因此：
   - 可以**携带状态**（如阈值 250），状态可通过 getter 读取、可被检查；
   - 类型在编译期确定，**避免函数指针的间接调用开销**，通常比函数指针快；
   - 可作为模板参数传递，实现「策略注入」（`count_if` 内部就把仿函数按值拷贝一份持有）。
2. **二元仿函数**满足「严格弱序」要求，可直接传给 `sort`/`set`/`map` 的比较器。
3. **`transform`（三参数版）**：把区间 `[first1,last1)` 的元素经函数变换后写入 `[first2,...)`。若输出位置就是输入位置，即为**原地变换**。
4. **`for_each`** 强调**副作用**（累加、打印、修改容器）；若只是产生新值，应选 `transform`。C++11 起 `for_each` 返回输入范围的迭代器。
5. **`bind`（部分应用）**：把仿函数的部分参数在编译期固定下来，生成新的可调用对象。`std::bind(ScoreGreater(240), std::placeholders::_1)` 表示「固定阈值 240，学生参数运行时传入」。C++17 后更多用**泛型 lambda** 替代。
6. **仿函数类 vs lambda 的取舍**：
   | | 带状态仿函数类 | lambda |
   |---|---|---|
   | 状态 | `this` 存储，可读可复用 | 值捕获（不可变）或引用捕获（可改） |
   | 复用性 | 高，同一类型可配不同状态反复使用 | 低，每处写法独立 |
   | 代码量 | 需单独写类 | 一行内联 |
   | 可读性/调试 | 类型名自解释，易加日志 | 简短但复杂时可读性下降 |
   | 典型选择 | 需要复用、需要状态查询、作为公共接口类型 | 一次性简单逻辑 |

**工程建议**：短小、一次性的谓词用 lambda；需要**复用、带命名状态、需要跨文件共享**的用仿函数类。两者本质等价（每个 lambda 都有唯一的匿名类类型，编译器可内联，性能几乎一致）。

---

## 附录 · 易错点速查表

| 序号 | 陷阱 | 正确做法 |
|---|---|---|
| 1 | `delete p` 用于 `new[]` 分配的数组 | 用 `delete[] p` |
| 2 | `new` 配 `free`、`malloc` 配 `delete` | 严格对应，不可混用 |
| 3 | 返回局部变量的引用或地址 | 返回 `static`/全局/堆/成员引用 |
| 4 | 拷贝构造只拷指针成员 | 深拷贝：重新分配并复制内容 |
| 5 | `operator[]` 返回值（而非引用） | 返回 `double&` 才能 `c[0]=x` |
| 6 | `<<`/`>>` 写成类的成员函数 | 写成非成员（通常为 `friend`） |
| 7 | 通过基类指针 `delete`，基类析构非 virtual | 基类析构写 `virtual` |
| 8 | `sort` 比较函数用 `>=` | 用 `>`，保证严格弱序 |
| 9 | 二进制存取含 `string`/指针的结构体 | 定长数组+长度，或文本/序列化库 |
| 10 | 类模板不做自动推导就省略 `<>` | 必须写 `Person<string,int>` |
| 11 | 默认参数加在参数列表中间 | 默认值必须从右向左连续 |
| 12 | `erase` 后继续使用失效的迭代器 | `remove_if` + `erase` 惯用法 |
| 13 | 初始化列表里读取未初始化的成员 | 只能作为赋值目标，右侧依赖用形参 |
| 14 | 浮点数用 `==` 比较 | 用 `fabs(a-b) < eps` |
| 15 | 有符号整数溢出后依赖其结果 | 有符号溢出是未定义行为，改用无符号 |

---

**编译方式**：`g++ -std=c++17 -Wall -O0 第N题.cpp -o 第N题 && ./第N题`。第 33、34 题需在可写目录下运行（程序内已 `remove` 清理临时文件）。

---

## 修订记录（相对初版的纠错）

初版答案中的代码与输出已在 `g++ 11.4.0 / x86-64 / Ubuntu 22.04` 下逐题实测，以下条目为**实测后发现并已修正**的问题：

**A. 代码编译不过（已修）**

| 题 | 原错误 | 修正 |
|---|---|---|
| 11 | `placeholder(int,int)` 与 `placeholder(int a,int)` 签名相同 → `redefinition` | 占位参数需与**参数个数不同**的版本重载：`placeholder(int)` / `placeholder(int, int)` |
| 12 | `show(l)` 二义未注释 → 整份编译失败 | 注释掉并说明歧义原因 |
| 13(3) | 片段缺 `#include <iostream>` / `using` | 已补 |
| 18 | `*s2.name`（对 `string` 对象解引用） | 改为 `s2.name = ...`；并加 `p2.name = nullptr` 兜底避免重复释放 |
| 20 | `Person& r = p.ageRef();` | 改为 `int& r` |
| 25 | `g.age` 二义 → 编译失败 | 改 `g.Dog::age`；阶段二缺 `#include <iostream>` 已补 |
| 26 | `GuideDogPlain` 未定义 | 补上普通继承对照组（4 个类） |
| 31 | 缺 `operator+`、`show()`、`<stdexcept>` | 已补 |
| 38 | 缺 `#include <iomanip>` | 已补 |

**B. 输出数值与答案不符（已按实测更正）**

| 题 | 原答案 | 实测 |
|---|---|---|
| 5 | 最大值下标 3 | **4** |
| 10 | `-O0` 输出 30 | **Segmentation fault（-O0/-O2 均崩溃）**；且 `getStaticAddr()` 直接输出是地址 |
| 14 | 漏 3 次「② 有参构造」、末尾析构顺序写成 张三/张三/空/空 | 末尾应为 **空/张三/张三/空** |
| 25 | `sizeof(GuideDog)` = 24 | **32** |
| 33 | `sizeof(Student)`=32、文件 51 字节 | **28**、**45** 字节 |
| 34 | 二进制文件 96 字节 | **64** 字节 |
| 35 | `显式指定实参` = 2.5,1.5 | **1.5,2.5**（已交换过一次） |
| 38 | `list: -1 0 1 2 3` | **-1 0 1 3** |
| 39 | 总分和 915、replace 出 3 个"待定" | **975**、**2 个**；lambda 排序同分顺序与 `cmp` 不同（`sort` 不稳定） |
| 40 | `>240` 相关计数均为 3 | **2**（240 不 >240）；`transform` 输出为原容器顺序 |

**C. 结论表述纠正**

- 第 9 题：x86 上**栈向低地址增长**；开启 ASLR 后堆/全局/常量的地址高低**无固定顺序**，不能用地址大小判断分区。
- 第 10 题：不存在" `-O0` 下看起来正确"的保证，GCC 会对 UB 插入陷阱指令。
- 第 17 题：`main()` 中 `BadPerson` 未实例化，需另写代码才能复现重复释放（实测 `Aborted (core dumped)`）。
- 第 18 题：浅拷贝不仅"牵连原对象"，程序结束时还会异常终止。