#ifndef ITRIWRITER_H
#define ITRIWRITER_H


template<typename VertexType>
class ITriWriter
{
public:
    virtual ~ITriWriter() {}

    virtual void push(const VertexType& v) = 0;
    virtual void push(const VertexType& v1, const VertexType& v2) = 0;
    virtual void push(const VertexType& v1, const VertexType& v2, const VertexType& v3) = 0;

    virtual void finish() = 0;
};


#endif // ITRIWRITER_H
