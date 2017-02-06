
#ifdef WIN32
#define ATOMIC_INC InterlockedIncrement
#define ATOMIC_DEC InterlockedDecrement
#define CAS(ptr, old_val, new_val) (InterlockedCompareExchange(ptr, new_val, old_val) == (old_val))
#else
#define ATOMIC_INC atomic_inc
#define ATOMIC_DEC atomic_dec
#define CAS(ptr, old_val, new_val) __sync_bool_compare_and_swap(ptr, old_val, new_val)
#endif

template<typename T, int _QUEUE_SIZE_ = 1024>
class spsc_queue
{
	T m_data[_QUEUE_SIZE_];
	volatile unsigned int m_head;
	volatile unsigned int m_tail;
private:
	spsc_queue(const spsc_queue&);
	spsc_queue& operator = (const spsc_queue&);

public:
	spsc_queue():m_head(0),m_tail(0){}

	bool  push(const T& data)
	{
		if (m_head+1 != m_tail)//not full
		{
			m_data[m_head++%_QUEUE_SIZE_] = data;
			return true;
		}
		return false;
	}

	bool pop(T& data)
	{
		if(m_head != m_tail)//not empty
		{
			data = m_data[m_tail++%_QUEUE_SIZE_];
			return true;
		}
		return false;
	}
};

template<typename T, int _QUEUE_SIZE_ = 1024>
class mpmc_queue
{
	T m_data[_QUEUE_SIZE_];
	volatile unsigned int m_head;
	volatile unsigned int m_tail;
	volatile unsigned int m_max_read_index;
private:
	mpmc_queue(const mpmc_queue&);
	mpmc_queue& operator = (const mpmc_queue&);

public:
	mpmc_queue():m_head(0),m_tail(0),m_max_read_index(0){}

	bool push(const T& data)
	{
		unsigned int cur_head;

		do
		{
			cur_head = m_head;
			if((cur_head+1) == m_tail)
			{
				return false;
			}

		}while(!CAS(&m_head, cur_head, cur_head+1));
			
		m_data[cur_head%_QUEUE_SIZE_] = data;

		while(!CAS(&m_max_read_index, cur_head, cur_head+1)){;}

		return true;
	}

	bool pop(T& data)
	{
		unsigned int cur_tail;

		do 
		{
			cur_tail = m_tail;
			if(cur_tail == m_max_read_index)
			{
				return false;
			}

			data = m_data[cur_tail%_QUEUE_SIZE_];

		} while (!CAS(&m_tail, cur_tail, cur_tail+1));

		return true;
	}
};
