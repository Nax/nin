#include <libnin/libnin.h>
#include <libnin/jit/jit.h>

NIN_API void ninJitBufferCreate(NinJitBuffer* buffer)
{
    buffer->size = 0;
    buffer->capacity = 64;
    buffer->data = malloc(64);
}

NIN_API void ninJitBufferDestroy(NinJitBuffer* buffer)
{
    free(buffer->data);
}

NIN_API void ninJitBufferPush(NinJitBuffer* buffer, const char* data, size_t len)
{
    size_t newCapacity;

    while (buffer->size + len > buffer->capacity)
    {
        newCapacity = buffer->capacity + buffer->capacity / 2;
        buffer->data = realloc(buffer->data, newCapacity);
        buffer->capacity = newCapacity;
    }

    memcpy(buffer->data + buffer->size, data, len);
    buffer->size += len;
}
