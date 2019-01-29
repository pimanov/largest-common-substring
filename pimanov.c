/**
 * @mainpage 
 * Данная программа находит наибольшую объщую подстроку среди нескольких
 * строк. Для этого по первой введенной строке строится сжатое суффиксное дерево. Далее, 
 * по каждой новой введеной строке находится общее поддерево уже существующего 
 * дерева и дерева, построенного по введеной строке. Соответственно в каждом 
 * узле есть отметка о том, какому потдереву он принадлежит. Последним шагом 
 * в поддереве, общем для всех введеных строк, для каждого узла находится 
 * длинна наибольшей подстроки, содержащейся в нем, и, двигаясь от корня в узлы с 
 * наиболшьшим значением данного параметра, можно найти наибольшую общую подстроку. 
 * 
 * Программа написанна на языке Си, стандарт с99. 
 * 
 * @author Пиманов Владмимр (Pimanov.Vladimir@gmail.com)
 * 
 * Программа решает второе задание для кондидатов в стажеры комании Yandex.
 * Условия задачи:
 *      Задание В. Наибольшая объщая подстрока
 *              Даны К строк, нужно найти их наибольшую общую подстроку.
 *      Формат входных данных
 *              В первой строке записанно целое число К (1 <= K <= 10). Далее 
 *              записанны исходные К строк. Каждая  строка состоит не более чем
 *              10000 маленьких латинских букв.
 *      Формат выходных данных
 *              Выведите наибольшую объшую подстроку
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LETTER_N 26 //число букв в алфавите
#define MAX_STR_N 10 //Максимальное число строк (не используется)
#define MAX_STR_LEN 10000 //максимальная длинна каждой строки

/**@note
 * Абривиатуры: 
 *      N - Number - чилсо
 *      ST - Suffix Tree - суффиксное дерево
 *      LCS - Longest Common Substring - наибольшая оюъщая подпоследовательность
 */

/**
 * @struct Node = ST_Node
 * Реализует узел суффиксного дерева. 
 * Ссылка на следующий узел выбирается из массива по значению первой буквы в 
 * подстроке, по которой следует выполнить переход к следующему узлу. 
 * Если ссылка пуста, ее значение NULL. В узле хранится вся подстрока, по которой 
 * выполняется переход в данный узел из предыдущего, вместе с первым символом.
 * Так же, хранится информация о том, для скольких суффиксных деревьев, 
 * построенных на вводимых строках, узел является общим. Храится информация о 
 * том, в каком потдереве содержится наибольшая подстрока.
 */
typedef struct Node
{
    /** ссылка на символ в строке, по которой построенно ST, с которого 
     * начинается подстрока, по которой в дереве осуществляется переход
     * к данному узлу. Символа '\0' в конце подстроки нет.  
     */
    const char* shiftStr;
    
    /** ссылка на символ, следующий за последним в подстроке shiftStr.
     */
    const char* shiftStrEnd;
    
    /**
     * У первой вводимой строки индекс 0, у каждой следующей индекс на 1 больше. 
     * ST, построенное по данной строке, имеен соответствующий индекс. 
     * Общее поддерево для ST с индексами от 0 до n, имеет индекс n.
     * Это поле хранит индекс последнего общего поддерева, которому принадлежит узел.
     */
    unsigned lastStrN;
    
    /** необходим для поиска наибольшей объщей подстроки. Пройдя от корня 
     * по указателям next[LcsRef] может быть получина LCS. 
     * Значение по умолчанию и null-указатель - это -1.
     */
    int LcsRef;
    
    /** Массив ссылок на дочерние узлы данного. Ажрес следющего узла - next[c-'a'], 
     * где с - значение первого символа подстроки, по которомй производится переход.
     * Елси в дереве нет узла, в который выполняется переход по символу с, значение 
     * ссылки next[c-'a'] = NULL
     */
    struct Node* next[MAX_LETTER_N];
} ST_Node;

/**@function newNode
 * возвращает ссылку на память, выделенную под лист Суффиксного Дерева, 
 * реализованный типом ST_Node. Память инициализируется. Так как в узле хранится 
 * информация о том, по какой подстроке производится переход в него, подстрока 
 * передается в параметрах для инициализации.
 * поле lastStrN = 0, поле LcsRef = -1. 
 * @param str - указатель на начало строки
 * @param strEnd - указатель на первый символ после конца строки
 * @return - ссылка на память, выделенную под структуру ST_Node и 
 * проинициализированную.
 */
ST_Node* newNode(const char* str, const char* strEnd);

/**
 * освобождает память, выделенную под структруру ST_Node. 
 * @param node - ссылка на ST_Node
 */
void deleteNode(ST_Node* node);

/**
 * Создает Суффиксное дерево. Возврящает ссылку на корень дерева. 
 * Строка, по которой строется дерево, не должна меняться, пока дерево не удалено. 
 * @param str - строка, по которой строится Суффиксное дерево.
 * @return - ссылка на корень нового дерева. 
 */
ST_Node* ST_init (const char* str);

/** вызывается в функции ST_init(). 
 * добавляет в дерево суффикс. Важно, чтобы каждый новый добавляемый суффикс 
 * был длинее всех уже добавленных. 
 * @param root - ссылка на корень дерева
 * @param str - начало строки, с которой храниться суффикс.
 * @param strEnd - указатель на символ за последним символом строки str
 * @return - ссылка на корень дерева, может измениться.
 */
ST_Node* ST_push (ST_Node* root, const char* str, const char* strEnd);

/**
 * выделяет в уже готовом дереве потдерево, объщее с деревом, построенным на
 * строке, переданной в параметрах.
 * @param root - ссылка на корень дерева
 * @param str - строка 
 * @return - кол-во строк, 
 */
unsigned int ST_putStr(ST_Node* root, const char* str);

/**
 * Используется в функции ST_putStr(). Не добовляет в дерево суффикс, но из 
 * общего поддерева всех деревьев, построенных на строках, введенных ранее, 
 * выделяет узлы, содержащие суффикс и отмечает их как узлы 
 * нового общего потдерева всех деревьев, построенных на предыдущих строках и данной.
 * @param root - ссыдка на корень дерева
 * @param str - добавляемая строка
 * @param strEnd - ссылка на символ, следующий после последнего в строке str
 * @param strN - номер добавляемой строки
 * @return - новая ссылка на корень дерева, старая может быть изменена
 */
ST_Node* ST_lazyPush(ST_Node* root, const char* str, const char* strEnd, unsigned int strN);

/**
 * удаляет дерево
 * @param node - ссылка на корень дерева
 */
void ST_free (ST_Node* node);

/**
 * заполняет поля LcsRef в узлах дерева, необходимые для того, что бы вывести 
 * наибольшую объщую подстроку(LCS) функцией ST_printLcs_r(). 
 * @param root - ссылка на корень дерева
 * @param strN - число строк, среди которых необходимо найти LCS, реализован 
 * несколько избыточный интерфейс. Используются строки с первой, на которой 
 * построенно дерево, по strN.
 * @return - длинна LCS
 */
unsigned int ST_indexLCSlen(ST_Node* root, unsigned int strN);

/**
 * рекурсивная функция, вызываемая в вункции ST_printLcs(), выводит в поток вывода 
 * самую длинную подстроку из проиндексированного ST. С каким параметром strN была 
 * произведина индексация, среди столькох строк и выводится наибольгая подстрока.
 * @param root - ссылка на корень дерева
 */
void ST_printLcs_r (const ST_Node* root);

/**
 * Печать наибольшей объщей подстроки среди всех строк, положенных в суффиксное дерево.
 * Индыксация вызывается в функции.
 * @param root - ссылка на корень дерева
 * @return - длинна наибольшей объщей подстроки
 */
unsigned int ST_printLcs(ST_Node* root);

/**
 * вывод на экран структуры дерева, в программе не используется. 
 * @param root - ссылка на корень дерева
 * @param tabsN - слежует указать 0
 */
void ST_print(const ST_Node* root, unsigned int tabsN);


ST_Node* ST_init (const char* str)
{
    const char* strEnd;
    for(strEnd = str; *strEnd != '\0'; strEnd++);
    
    ST_Node* root = NULL;
    for(const char* suffix = strEnd; suffix >= str; --suffix) 
        root = ST_push(root, suffix, strEnd);
    
    //узел root содержит пустую строку перехода, даже если строка, по которой
    //построенно дерево, пуста
    return root;
}

ST_Node* ST_push (ST_Node* root, const char* str, const char* strEnd)
{
    if (root == NULL) return newNode(str, strEnd);
    
    //сравнение добавляемой строки с той, по которой происходит переход в этот узел
    const char* shift; 
    for(shift = root->shiftStr ;shift != root->shiftStrEnd; shift++, str++) 
        if(*str != *shift) break;
    
    if(shift == root->shiftStrEnd) { // строки совпали
        /*if(*str != '\0') // не нужно, так как все суффиксы разной длинны*/ 
        root->next[*str - 'a'] = 
            ST_push(root->next[*str - 'a'], str, strEnd);
        return root;
    }
    
    //строки не совпали, нужно резать строку-стрелку и создавать новый узел
    ST_Node* node = newNode(root->shiftStr, shift);
    root->shiftStr = shift;
    node->next[*shift - 'a'] = root;
    /*if(*str != '\0') //не нужно если суффиксы вводятся от короткого к длинному */ 
        node->next[*str - 'a'] = newNode(str, strEnd);
    return node;
}


unsigned int ST_putStr(ST_Node* root, const char* str)
{
    const char* strEnd;
    for(strEnd = str; *strEnd != '\0'; strEnd++);
    
    unsigned int lastStrN = root->lastStrN;
    for(const char* suffix = strEnd; suffix >= str; --suffix) 
        root = ST_lazyPush(root, suffix, strEnd, lastStrN);
    
    return root->lastStrN + 1;
}

ST_Node* ST_lazyPush(ST_Node* root, const char* str, const char* strEnd, unsigned int strN)
{
    // то же, что и в ST_push(), но не создаются новые листья, а модифицируется
    // lastStrN параметр узла, через которое проходит суффикс.
    if (root == NULL) return NULL;
    
    //Если условие выполнено, узел не принадлежит общему поддереву, построенному 
    //на всех уже введеных строках  
    if (root->lastStrN < strN) return root;
    
    const char* shift; 
    for (shift = root->shiftStr ;shift != root->shiftStrEnd; shift++, str++) 
        if (*str != *shift) break;

    if (shift == root->shiftStrEnd) {
        root->lastStrN = strN+1;
        if(*str != '\0') root->next[*str - 'a'] = 
            ST_lazyPush(root->next[*str - 'a'], str, strEnd, strN);
        return root;
    }

    ST_Node* node = newNode(root->shiftStr, shift);
    node->lastStrN = strN+1;
    root->shiftStr = shift;
    node->next[*shift - 'a'] = root;
    return node;
}


ST_Node* newNode(const char* str, const char* strEnd)
{
    ST_Node* node = (ST_Node*) malloc(sizeof(ST_Node));
    node->shiftStr = str;
    node->shiftStrEnd = strEnd;
    node->lastStrN = 0;
    node->LcsRef = -1;
    for(int i=0; i<MAX_LETTER_N; i++) node->next[i] = NULL;
    
    return node;
}

void deleteNode(ST_Node* node)
{
    free(node);
}

void ST_free (ST_Node* node)
{
    for(int i=0; i<MAX_LETTER_N; i++) if(node->next[i] != NULL) ST_free(node->next[i]);
    deleteNode(node);
}

unsigned int ST_indexLCSlen(ST_Node* root, unsigned int strN)
{
    // Проходим только по общему поддереву всех деревьев, построенных на первых 
    // strN+1 строках с индексами от 0 до strN.
    if(strN <= root->lastStrN)
    {
        unsigned int LcsLen = 0;
        root->LcsRef = -1;

        for(int i=0; i<MAX_LETTER_N; i++) if(root->next[i] != NULL) 
        {
            unsigned int iLcsLen = ST_indexLCSlen(root->next[i], strN);
            if( iLcsLen > LcsLen) {
                LcsLen = iLcsLen;
                root->LcsRef = i;
            }
        }

        for(const char* c = root->shiftStr; c != root->shiftStrEnd; c++) LcsLen ++;
        return LcsLen;
    }
    else return 0;
}


void ST_printLcs_r (const ST_Node* root)
{
    for(const char* c = root->shiftStr; c != root->shiftStrEnd; c++) printf("%c",*c);
    if(root->LcsRef >= 0) ST_printLcs_r(root->next[root->LcsRef]);
}

unsigned int ST_printLcs(ST_Node* root)
{
    unsigned int LcsLen = ST_indexLCSlen(root, root->lastStrN);
    ST_printLcs_r(root);
    printf("\n");
    return LcsLen;
}


void ST_print(const ST_Node* root, unsigned int tabsN)
{
    for(unsigned int tabN = 0; tabN < tabsN; tabN++) printf("  ");
    for(const char* c = root->shiftStr; c != root->shiftStrEnd; c++) printf("%c",*c);
    printf(" %d\n", root->lastStrN);
    for(int i=0; i<MAX_LETTER_N; i++) if(root->next[i] != NULL) ST_print(root->next[i],tabsN+1);
}

int main () 
{
    char str[MAX_STR_LEN +1], buf[MAX_STR_LEN +1];
    unsigned int strsN;
    

    gets(buf);
    strsN = atoi(buf);
    
    gets(str);
    ST_Node* root = ST_init(str);
    
    for(int strN=1; strN<strsN; strN++)
    {
        gets(buf);
        ST_putStr(root, buf);
    }
    
    ST_printLcs(root);
    
//    ST_print(root,0);
    ST_free(root);
    
    return 0;
}
