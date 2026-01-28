//if first time
#ifndef VBUFFER_H
#define VBUFFER_H false
#endif

#if (VBUFFER_H == false)
#pragma warning(push)
#pragma warning(disable : 4005)
/*
These are macros that can be issued as a type specifier for a template function issued in a 'Substitution Failure Is Not An Error' form.
They serve for making the code more readable, and making the purpose of it clearer..
Usage examples:

	(header file)
	class X {
		template <typename T>
		static SFINAE_POPULATE_VERTEX_T PopulateBufferObject(vBuffer& buffer, const std::vector<T>& srcData);
	};

	(source file)
	template <typename T>
	SFINAE_POPULATE_VERTEX_T X::PopulateBufferObject(vBuffer& buffer, const std::vector<T>& srcData)
	{
		... do stuff
	}

	WHY? would you like it to look like this?: (.inl version equates to)
	template<typename T> typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, Vertex>::value>::type FUNCTION_NAME(PARAMS)
*/
#define VERTEX_TYPE		std::is_same<T, VWrapper::Vertex>::value
#define INTEGRAL_TYPE	std::is_integral<T>::value
#define SFINAE_POPULATE_VERTEX_T		typename std::enable_if<!INTEGRAL_TYPE &&  VERTEX_TYPE>::type
#define SFINAE_POPULATE_INTEGRAL_T		typename std::enable_if< INTEGRAL_TYPE && !VERTEX_TYPE>::type
#define SFINAE_POPULATE_UNSUPPORTED_T	typename std::enable_if<!INTEGRAL_TYPE && !VERTEX_TYPE>::type

#define VBUFFER_H true
#pragma warning(pop)
#endif
/*
To fully undefine the above macros use the contents of this comment
#pragma warning(push)
#pragma warning(disable : 4005)
#define VBUFFER_H false
#undef VERTEX_TYPE
#undef INTEGRAL_TYPE
#undef SFINAE_POPULATE_VERTEX_T
#undef SFINAE_POPULATE_INTEGRAL_T
#undef SFINAE_POPULATE_UNSUPPORTED_T
#pragma warning(pop)
*/