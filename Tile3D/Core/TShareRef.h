#pragma once


//1) not thread safe
//
template<typename T> class TShareRef
{
public:
	TShareRef(T* object){
		ref_object = object;
		ref_count = new int();
		*ref_count = 1;
	} 
	

	~TShareRef(){
		Dispose();
	}

	void Dispose()
	{
		DecRef();
		if(GetRef() == 0)
		{
			delete ref_object;
			delete ref_count;
		}

	}

	//copy ctor
	TShareRef(const TShareRef & ref){
		this->ref_object = ref.ref_object;
		this->ref_count = ref.ref_count;
		AddRef();
	}


	//move ctor
	TShareRef & operator= (const TShareRef & ref){
		if(this == &ref) return *this;
		Dispose();

		this->ref_object = ref.ref_object;
		this->ref_count = ref.ref_count;
		AddRef();
		return *this;
			
	}

	T * Get(){
		return ref_object;
	}


	T* operator->(){
		return ref_object;
	}

	T & operator*(){
		return *ref_count;
	}

	void DecRef(){
		*ref_count = *ref_count -1;
	}


	void AddRef(){
		*ref_count = *ref_count +1;

	}

	int GetRef(){
		return *ref_count;
		
	}


private:
	T* ref_object;
	int * ref_count;	


private:
	TShareRef(){}

};

