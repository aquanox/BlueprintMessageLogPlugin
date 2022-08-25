#pragma once

// from NetworkUtilTests
// Hack for accessing private members in various Engine classes

// This template, is used to link an arbitrary class member, to the GetPrivate function
template<typename Accessor, typename Accessor::Member Member>
struct AccessPrivate
{
	friend typename Accessor::Member GetPrivate(Accessor InAccessor)
	{
		return Member;
	}
};

// Need to define one of these, for every member you want to access (preferably in the .cpp) - for example:
#if 0
// This is used to aid in linking one member in FStackTracker, to the above template
struct FStackTrackerbIsEnabledAccessor
{
	using Member = bool FStackTracker::*;

	friend Member GetPrivate(FStackTrackerbIsEnabledAccessor);
};

// These combine the structs above, with the template for accessing private members, pointing to the specified member
template struct AccessPrivate<FStackTrackerbIsEnabledAccessor, &FStackTracker::bIsEnabled>;
#endif

// Example for calling private functions
#if 0
// Used, in combination with another template, for accessing private/protected members of classes
struct AShooterCharacterServerEquipWeaponAccessor
{
	using Member = void (AShooterCharacter::*)(AShooterWeapon* Weapon);

	friend Member GetPrivate(AShooterCharacterServerEquipWeaponAccessor);
};

// Combines the above struct, with the template used for accessing private/protected members
template struct AccessPrivate<AShooterCharacterServerEquipWeaponAccessor, &AShooterCharacter::ServerEquipWeapon>;

// To call private function:
//	(GET_PRIVATE(AShooterCharacter, CurChar, ServerEquipWeapon))(AShooterWeapon::StaticClass());
#endif

/**
 * Defines a class and template specialization, for a variable, needed for use with the GET_PRIVATE hook below
 *
 * @param InClass		The class being accessed (not a string, just the class, i.e. FStackTracker)
 * @param VarName		Name of the variable being accessed (again, not a string)
 * @param VarType		The type of the variable being accessed
 */
#define IMPLEMENT_GET_PRIVATE_VAR(InClass, VarName, VarType) \
	struct InClass##VarName##Accessor \
	{ \
		using Member = VarType InClass::*; \
		\
		friend Member GetPrivate(InClass##VarName##Accessor); \
	}; \
	\
	template struct AccessPrivate<InClass##VarName##Accessor, &InClass::VarName>;

// @todo #JohnB: Unfortunately, this broke in VS2015 for functions, as '&InClass::FuncName' gives an error;
//					strictly speaking, this falls within the C++ standard and should work, so perhaps make a bug report
//					See SLogWidget.cpp, for an alternative way of hack-accessing private members, using template specialization
#if 0
/**
 * Defines a class and template specialization, for a function, needed for use with the GET_PRIVATE hook below
 *
 * @param InClass		The class being accessed (not a string, just the class, i.e. FStackTracker)
 * @param FuncName		Name of the function being accessed (again, not a string)
 * @param FuncRet		The return type of the function
 * @param FuncParms		The function parameters
 * @param FuncModifier	(Optional) Modifier placed after the function - usually 'const'
 */
#define IMPLEMENT_GET_PRIVATE_FUNC_CONST(InClass, FuncName, FuncRet, FuncParms, FuncModifier) \
	struct InClass##FuncName##Accessor \
	{ \
		using Member = FuncRet (InClass::*)(FuncParms) FuncModifier; \
		\
		friend Member GetPrivate(InClass##FuncName##Accessor); \
	}; \
	\
	template struct AccessPrivate<InClass##FuncName##Accessor, &InClass::FuncName>;

#define IMPLEMENT_GET_PRIVATE_FUNC(InClass, FuncName, FuncRet, FuncParms) \
	IMPLEMENT_GET_PRIVATE_FUNC_CONST(InClass, FuncName, FuncRet, FuncParms, ;)
#endif


/**
 * A macro for tidying up accessing of private members, through the above code
 *
 * @param InClass		The class being accessed (not a string, just the class, i.e. FStackTracker)
 * @param InObj			Pointer to an instance of the specified class
 * @param MemberName	Name of the member being accessed (again, not a string)
 * @return				The value of the member
 */
#define GET_PRIVATE(InClass, InObj, MemberName) (*InObj).*GetPrivate(InClass##MemberName##Accessor())

/**
 * Redundant version of the above, for emphasizing the ability to assign a writable reference which can modify the original value
 */
#define GET_PRIVATE_REF(InClass, InObj, MemberName) GET_PRIVATE(InClass, InObj, MemberName)


// @todo #JohnB: Restore if fixed in VS2015
#if 0
// Version of above, for calling private functions
#define CALL_PRIVATE(InClass, InObj, MemberName) (GET_PRIVATE(InClass, InObj, MemberName))
#endif


/**
 * Defines a class used for hack-accessing protected functions, through the CALL_PROTECTED macro below
 *
 * @param InClass		The class being accessed (not a string, just the class, i.e. FStackTracker)
 * @param FuncName		Name of the function being accessed (again, not a string)
 * @param FuncRet		The return type of the function
 * @param FuncParms		The function parameters
 * @param FuncParmNames	The names of the function parameters, as passed from one function to another
 * @param FuncModifier	(Optional) Modifier placed after the function - usually 'const'
 */
#define IMPLEMENT_GET_PROTECTED_FUNC_CONST(InClass, FuncName, FuncRet, FuncParms, FuncParmNames, FuncModifier) \
	struct InClass##FuncName##Accessor : public InClass \
	{ \
		FORCEINLINE FuncRet FuncName##Accessor(FuncParms) FuncModifier \
		{\
			return FuncName(FuncParmNames); \
		} \
	};

#define IMPLEMENT_GET_PROTECTED_FUNC(InClass, FuncName, FuncRet, FuncParms, FuncParmNames) \
	IMPLEMENT_GET_PROTECTED_FUNC_CONST(InClass, FuncName, FuncRet, FuncParms, FuncParmNames,)

// Version of GET_PRIVATE, for calling protected functions
#define CALL_PROTECTED(InClass, InObj, MemberName) ((InClass##MemberName##Accessor*)&(*InObj))->MemberName##Accessor

