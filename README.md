# Ускорение построения множества мандельброта с использованием SIMD.
**Цели работы**
- Рассчитать множество мандельброта
- Ускороить расчет с помощью SIMD и intrinsic функций

  ## Аннотация
  Множество мандельброта задается на комлексной плоскости следующим уравнением


  $z_n = z_{n - 1}^2 + z_0$

  **SIMD (Single Instruction Multiple Data)** - принцип компьютерных вычислений, позволяющий обеспечить параллелизм на уровне данных.

- Для визуализации используем библиотеку SDL.
- Используем компилятор ``gcc`` с флагами ```-O2``` или ```-O3```
- Для линковки библиотеки SDL используем флаги ```-lSDL2, -lSDL2_image```
- Для подключения intrinsic функций используем флаги ```-mavx2, -mavx2, -mfma, -mavx512vl```

  ## Ход работы
  Для начала напишем стандартную реализацию построения множества Мандельброта без SIMD

```c
    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SDL_SCREEN_WIDTH; x++)
        {
            float x0 = -1.f + x * 2.f / SDL_SCREEN_WIDTH;

            float y0 = -1.f + y * 2.f / SDL_SCREEN_HEIGHT;

            float z_x    = 0;
            float z_y    = 0;
            float z_x2   = 0;
            float z_y2   = 0;
            float rmax   = 4;

            volatile int it = 0;
            for (; it < Niter; it++)
            {
                z_y = 2 * z_y * z_x + y0;
                z_x = z_x2 - z_y2 + x0;

                if (z_x2 + z_y2 > rmax)
                {
                    *dots_pointer = it;
                    dots_pointer++;
                    break;
                }

                z_x2 = z_x * z_x;
                z_y2 = z_y * z_y;

            }
        }
    }
  ```

#### Реализация с AVX функциями
```c

    alignas(32) __m256 mdely = _mm256_set1_ps(dely);
    alignas(32) __m256 mdelx = _mm256_set1_ps(delx);
    alignas(32) __m256 dy =   _mm256_set1_ps(param->stpy);
    alignas(32) __m256 dx =   _mm256_set1_ps(param->stpx * param->pksz);
    alignas(32) __m256 sdx =  _mm256_set1_ps(param->stpx);
    alignas(32) __m256 bias = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
    alignas(32) __m256 rmax = _mm256_set1_ps(param->rmax);


    alignas(32) __m256 y0 = _mm256_set1_ps(-1.0f);

    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++, y0 = _mm256_add_ps(y0, dy))
    {

        alignas(32) __m256 x0 = _mm256_add_ps(_mm256_mul_ps(bias, sdx), _mm256_set1_ps(-1.f));

        for (int x = 0; x < SDL_SCREEN_WIDTH; x += param->pksz, x0 = _mm256_add_ps(x0, dx))
        {
            alignas(32) __m256 z_x      = _mm256_setzero_ps();
            alignas(32) __m256 z_y      = _mm256_setzero_ps();
            alignas(32) __m256 z_x2     = _mm256_setzero_ps();
            alignas(32) __m256 z_y2     = _mm256_setzero_ps();
            alignas(32) __m256i iter    = _mm256_setzero_si256();

            for (int i = 0; i < Niter; i++)
            {
                z_y = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(z_y, z_x), _mm256_set1_ps(2)), _mm256_add_ps(_mm256_mul_ps(y0, _mm256_set1_ps(ampl)), mdely));
                z_x = _mm256_add_ps(_mm256_sub_ps(z_x2, z_y2), _mm256_add_ps(_mm256_mul_ps(x0, _mm256_set1_ps(ampl)), mdely));

                __m256 r2 = _mm256_add_ps(z_x2, z_y2);
                __m256 mask = _mm256_cmp_ps(r2, rmax, _CMP_LT_OS);
                iter = _mm256_sub_epi32(iter, _mm256_castps_si256(mask);
                int mask2int = _mm256_movemask_ps(mask);
                if (mask2int == 0x00)
                {
                    alignas(32) int integers[8] = {};
                    _mm256_store_epi32(integers, iter);

                    for (int i = 0; i < param->pksz; i++)
                        dots[(x + i) + y * SDL_SCREEN_HEIGHT] = integers[i];

                    break;
                }
                z_x2 = _mm256_mul_ps(z_x, z_x);
                z_y2 = _mm256_mul_ps(z_y, z_y);
            }
        }
    }
```

### Измерения
- Проводим измерения на компиляторе g++ 20-го стандарта с флагами -O2
  
- Измерена средняя продолжительность одной итерации подсчета мандельброта

- 
  ![3Ghz](https://github.com/user-attachments/assets/4408d727-fa89-4fb2-a3b5-89be57f91719)

Измерения сделаны при частоте процессора в 3Ghz

- DEFAULT:  3.74s 
- ARRAY:    0.96s
- SSE:      0.99s
- AVX:      0.48s
---
- AVX ускорил в 7.8 раз
- SSE ускорил в 3.7 раз
- ARRAY ускорил в 3.8 раз

---
- Проведено второе измерение при той же частоте на следующий день, но результаты существенно отличаются (сделано 1000 повторов для каждого эксперимента)
 
![3Ghz2](https://github.com/user-attachments/assets/e8615a1f-9ac7-4d72-a8bf-aa0b726fea67)

![image](https://github.com/user-attachments/assets/a103f420-8d1b-480c-b70f-c65a49749305)

$\sigma_{256} = 6 * 10^{-7}$

$T_{avg} = 0.0654670 ± 6 * 10^{-7}$ с

![image](https://github.com/user-attachments/assets/5d225c4e-af7f-406a-a61a-e105b0857772)

$\sigma_{128} = 8 * 10^{-7}$

$T_{avg} = 0.1304322 ± 8 * 10 ^{-7}$ с

![image](https://github.com/user-attachments/assets/764fa1d1-8427-41d0-af20-f02158bfdb18)

$\sigma_{128} = 2 * 10^{-6}$

$T_{avg} = 0.12503 ± 2 * 10^{-6}$ с

![image](https://github.com/user-attachments/assets/eb2dbfa8-7e72-4049-a35d-2d9c611c9405)

$\sigma_{128} = 1.3 * 10^{-5}$

$T_{avg} = 0.47315 ± 1.3 * 10^{-5}$ с

---
- ARRAY ускорил в 3.78 раз
- SSE ускорил в   3.63 раз
- AVX ускорил в   7.23 раз

---
## Выводы
- Проведены измерения подсчета множества Мандельброта с использованием intrinsic-функций. Проведено два независимых исследования, выполненных при частоте процессора 3Ghz, но имеющих совершенно разные результаты.

В обоих случаях AVX функции ускоряют примерно в 7 раз, а SSE в 3.5



  

