#include "cstack.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(VAR) (void)(VAR)

struct Stack{
    struct StackElement* lastElPtr;
};

struct StackElement{
    struct StackElement* prevElement;
    unsigned int size;
    void* data;
};

struct StackElement* create_stack_element(void* data, unsigned int size, struct StackElement* prevElement){

    // Проверка на адекватность входных данных
    if ((data == NULL) || (size == 0)){
        return NULL;
    }
    struct StackElement* element = malloc(sizeof(struct StackElement));

    element->data = malloc(size);
    
    memcpy(element->data, data, size);
    element->size = size;
    element->prevElement = prevElement;
    
    return element;
}

// Создаём stackArray
static int stackArrayCount = 0;
static struct Stack** stackArray = NULL;

hstack_t stack_new(void)
{
    // Добавление стека
    struct Stack* stack = malloc(sizeof(struct Stack)); // Выделяем память
    stack->lastElPtr = NULL;  // Элементов в стеке пока что нет

    stackArray = realloc(stackArray, (stackArrayCount + 1) * sizeof(struct Stack*));
    stackArray[stackArrayCount] = stack;

    int new_handle = stackArrayCount;
    stackArrayCount += 1;
    return new_handle;
}

void stack_free(const hstack_t hstack)
{
    // Удаление стека и его элементов

    // Проверка на адекватность id стека
    if (stack_valid_handler(hstack) == 1){
        return;
    }


    struct Stack** newStackArray = NULL;
    if (stackArrayCount > 1){
       newStackArray = malloc((stackArrayCount - 1) * sizeof(struct Stack*));
    }
        
    int newIdx = 0;
    for (int i=0; i < stackArrayCount; i++){
        if (i==hstack){
            // Нужно удалить все элементы рассматриваемого стека
            struct Stack* deletingStack = stackArray[i];
            struct StackElement* el = deletingStack->lastElPtr;
            while(el != NULL){
                struct StackElement* nextEl = el->prevElement;
                free(el->data);
                free(el);
                el = nextEl;
            }
            free(deletingStack);
            continue;
        }
        newStackArray[newIdx] = stackArray[i];
        newIdx++;
    }
    free(stackArray);
    stackArray = newStackArray;
    stackArrayCount -= 1;

}

int stack_valid_handler(const hstack_t hstack)
{
    if ((stackArrayCount == 0) || (hstack < 0) || (hstack >= stackArrayCount)){
        return 1;
    }
    else {
        return 0;
    }
}

unsigned int stack_size(const hstack_t hstack)
{
    // Проверка на адекватность id стека
    if (stack_valid_handler(hstack) == 1){
        return 0;
    }

    int count = 0;
    struct Stack* currentStack = stackArray[hstack];
    struct StackElement* el = currentStack->lastElPtr;
    while(el != NULL){
        count += 1;
        el = el->prevElement;
    }
    return count;
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    // Проверка на адекватность id стека
    if (stack_valid_handler(hstack) == 1){
        return;
    }
    struct Stack* currentStack = stackArray[hstack];
    struct StackElement* lastEl = currentStack->lastElPtr;
    struct StackElement* el = create_stack_element((void*)data_in, size, lastEl);
    if (el != NULL) {
        currentStack->lastElPtr = el;
    }
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    // Проверка на декватность id стека
    if (stack_valid_handler(hstack) == 1) {
        return 0;
    }
    
    struct Stack* currentStack = stackArray[hstack];
    struct StackElement* lastEl = currentStack->lastElPtr;
    
    // Если стек пустой
    if (lastEl == NULL) {
        return 0;
    }
    
    unsigned int sizeOfEl = 0;
    
    // Копируем данные в буфер, если он не NULLPTR
    if (data_out != NULL && size > 0) {
        sizeOfEl = (size < lastEl->size) ? size : lastEl->size;
        memcpy(data_out, lastEl->data, sizeOfEl);
    } else {
        // Если буфера нет, то элемент из стека все равно удалим
        sizeOfEl = lastEl->size;
    }
    
    // Обнавляем указатель на последний элемент
    struct StackElement* preLastEl = lastEl->prevElement;
    currentStack->lastElPtr = preLastEl;
    
    // Освобождаем память элемента
    free(lastEl->data);
    free(lastEl);
    
    return sizeOfEl;
}

