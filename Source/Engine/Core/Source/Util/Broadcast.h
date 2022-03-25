#pragma once
#include <functional>
#include "../Containers/Array.h"

using Faro::Array;

#define NOARG

#define MERGE2(A, B) A,B
#define MERGE3(A, B, C) A,B,C
#define MERGE4(A, B, C, D) A,B,C,D

#define BROADCAST_BASE(NAME, TYPES, PARAMETERS, VARIABLES) class NAME \
{ \
	public: \
	void Bind(std::function<void(TYPES)> func){ binds_.Add(func); } \
	void Dispatch(PARAMETERS) { \
		for(std::function<void(TYPES)>& func : binds_){ func(VARIABLES); } \
	} \
	\
	private: \
	Array<std::function<void(TYPES)>> binds_; \
};

#define DECLARE_BROADCAST(NAME) BROADCAST_BASE(NAME, NOARG, NOARG, NOARG)
#define DECLARE_BROADCAST_1(NAME, TYPE) BROADCAST_BASE(NAME, TYPE, TYPE Var1, Var1)
#define DECLARE_BROADCAST_2(NAME, TYPE1, TYPE2) BROADCAST_BASE(NAME, MERGE2(TYPE1, TYPE2), MERGE2(TYPE1 Var1, TYPE2 Var2), MERGE2(Var1, Var2))
#define DECLARE_BROADCAST_3(NAME, TYPE1, TYPE2, TYPE3) BROADCAST_BASE(NAME, MERGE3(TYPE1, TYPE2, TYPE3), MERGE3(TYPE1 Var1, TYPE2 Var2, TYPE3 Var3), MERGE3(Var1, Var2, Var3))
#define DECLARE_BROADCAST_4(NAME, TYPE1, TYPE2, TYPE3, TYPE4) BROADCAST_BASE(NAME, MERGE4(TYPE1, TYPE2, TYPE3, TYPE4), MERGE4(TYPE1 Var1, TYPE2 Var2, TYPE3 Var3, TYPE4 Var4), MERGE4(Var1, Var2, Var3, Var4))
