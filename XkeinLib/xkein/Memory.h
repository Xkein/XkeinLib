#pragma once

#include "DataStruct.h"
#include "tools.h"

XKEINNAMESPACE_START

	class MemoryBuffer
	{
	public:
		static MemoryBuffer Apply(size_t size)
		{
			MemoryBuffer buffer;
			buffer.buffer = ::operator new(size);
			buffer.size = size;
			return buffer;
		}

		static void Dispose(MemoryBuffer& buffer)
		{
			::operator delete(buffer.buffer);
			buffer.buffer = nullptr;
			buffer.size = 0;
		}

		bool Applied() const _NOEXCEPT
		{
			return buffer != nullptr && size;
		}

		template<class _Ty = void*>
		_Ty GetBuffer() const _NOEXCEPT
		{
			return Convert<_Ty>(buffer);
		}

		size_t GetSize() const _NOEXCEPT
		{
			return size;
		}

	protected:
		void* buffer;
		size_t size;
	};

	class MemoryManager
	{
	public:
		template<class _Ty, class _Ty2>
		struct AllocType {
			using type = _Ty;
		};
		template<class _Ty>
		struct AllocType<void, _Ty> {
			using type = _Ty;
		};
		template<class _Ty = void, class _Ty2 = _Ty>
		MemoryBuffer& AllocMemory(_Ty2*& ptr = GetNullReference<_Ty2*>())
		{
			MemoryBuffer& buffer = AllocMemory(sizeof(typename AllocType<_Ty, _Ty2>::type));
			if (&ptr) {
				ptr = buffer.GetBuffer<_Ty2*>();
			}
			return buffer;
		}

		virtual MemoryBuffer& AllocMemory(size_t size)
		{
			if (!unused_memory.empty()) {
				MemoryBuffer& buffer = *unused_memory.top();
				unused_memory.Pop();
				buffer = MemoryBuffer::Apply(size);
				return (buffer);
			}
			memory.PushBack(MemoryBuffer::Apply(size));
			return (memory.back());
		}

		virtual void FreeMemory(MemoryBuffer& buffer)
		{
			if (buffer.Applied()) {
				Free(buffer);
				unused_memory.Push(&buffer);
			}
		}

		virtual void FreeMemory(void* buffer)
		{
			if (buffer != nullptr) {
				for (MemoryBuffer& rBuffer : memory)
				{
					if (rBuffer.GetBuffer() == buffer) {
						FreeMemory(rBuffer);
					}
				}
			}
		}

		virtual ~MemoryManager()
		{
			for (MemoryBuffer& rBuffer : memory)
			{
				MemoryBuffer::Dispose(rBuffer);
			}
			memory.Clear();
		}

	protected:
		virtual void Free(MemoryBuffer& buffer)
		{
			MemoryBuffer::Dispose(buffer);
		}

	protected:
		DynamicArray<MemoryBuffer> memory;
		Stack<MemoryBuffer*> unused_memory;
	};

XKEINNAMESPACE_END