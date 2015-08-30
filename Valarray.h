// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h
#include <vector>
#include "Vector.h"
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <complex>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <typeinfo>

using epl::vector; // during development and testing
// using epl::vector; // after submission
namespace epl {
    
    
    
    
    template <int R>
    struct stype;
    
    template <> struct stype<1> {using type=int;};
    template <> struct stype<2> {using type=float;};
    template <> struct stype<3> {using type=double;};
    
    template <typename T> struct iscomplex: public std::false_type {};
    template <typename T> struct iscomplex<std::complex<T>>: public std::true_type {};
    
    template <bool p, typename T> struct ctype;
    template<typename T> struct ctype<true, T> {using type=std::complex<T>;};
    template<typename T> struct ctype<false, T> {using type=T;};
    
    
    template <typename T>
    struct rank;
    
    template <> struct rank<int> { static constexpr int value = 1; };
    template <> struct rank<float> { static constexpr int value = 2; };
    template <> struct rank<double> { static constexpr int value = 3; };
    template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };
    
    template<typename B>
    class val;
    
    template <typename T1, typename T2>
    struct choose_type{
        
        static constexpr int rankt1=rank<T1>::value;
        static constexpr int rankt2=rank<T2>::value;
        static constexpr int maxrank=(rankt1<rankt2)?rankt2:rankt1;
        
        using my_stype=typename stype<maxrank>::type;
        
        static constexpr bool iscom_T1=iscomplex<T1>::value;
        static constexpr bool iscom_T2=iscomplex<T2>::value;
        static constexpr bool iscom=iscom_T1||iscom_T2;
        
        using type=typename ctype<iscom,my_stype>::type;
        
    };
    
    template <typename T1, typename T2>
    struct choose_type<val<T1>,T2>{
        
        using type=T1;
        
    };
    
    template <typename T1, typename T2>
    struct choose_type<T1,val<T2>>{
        
        using type=T1;
        
    };
    
    template <typename T1, typename T2>
    struct choose_type<val<T1>,val<T2>>{
        using type=T1;
        
    };
    template<typename left, typename right, typename fun>
    class proxy;
    
    template<typename T>
    struct val_type{
        using type=T;
    };
    
    template<typename T>
    struct val_type<val_type<T>>{
        using type=T;
    };
    
    template<typename T>
    struct to_ref{
        using type=const T;
    };
    
    template<typename T>
    struct to_ref<vector<T>>{
        using type=const vector<T>&;
    };
    
    template<typename T>
    struct scalar;
    
    template<typename T>
    struct unwrap{
        using type=T;
    };
    
    template<typename T>
    struct unwrap<val<T>>{
        using type=typename T::value_type;
    };
    
    
    template<typename T>
    struct prepare{
        //        using type=scalar<T>;
    };
    
    template<>
    struct prepare<int>{
        using type=scalar<int>;
    };
    
    template<>
    struct prepare<double>{
        using type=scalar<double>;
    };
    
    template<>
    struct prepare<float>{
        using type=scalar<float>;
    };
    
    template<>
    struct prepare<std::complex<float>>{
        using type=scalar<std::complex<float>>;
    };
    
    template<>
    struct prepare<std::complex<double>>{
        using type=scalar<std::complex<double>>;
    };
    
    template<typename T>
    struct prepare<val<T>>{
        using type=T;
    };

    
    template<typename T1, typename T2>
    struct add;
    
    template<typename T>
    class scalar {
    public:
        uint64_t s;
        T value;
        using value_type=T;
        const T operator[](uint64_t) const{
            return value;
        }
        
        uint64_t size() const{
            return s;
        }
        scalar(T t){
            value=t;
            s=std::numeric_limits<uint64_t>::max();
        }
     
    };
    
/***************************************************************************************************************************/
    /******* proxy for unary operation  ***/
    template<typename left, typename fun>
    class proxy_1{
    public:
        fun f;
        typename  to_ref<left>::type l;
        uint64_t s;
        
    
        proxy_1(const proxy_1& rhs):l(rhs.l),f(rhs.f),s(rhs.s){
        }
        proxy_1(const left& ll, fun& ff):l(ll),f(ff),s(ll.size()){
        }
         
        

        using value_type=typename fun::result_type;
        
        value_type operator[](uint64_t i){
            if(i>=s){
                throw std::out_of_range{"Index out of boundary!"};
            }else{
                return f(l[i]);
            }
        }
        
        const value_type operator[](uint64_t i) const{
            if(i>=s){
                throw std::out_of_range{"Index out of boundary!"};
            }else{
                return f(l[i]);
            }
        }
        
        
        uint64_t size() const{
            return s;
        }
        class iterator{
            uint64_t index;
            proxy_1 p;
        public:
            iterator(uint64_t i, proxy_1 pro):p(pro){
                index=i;
            }
            void operator++(void){
                ++index;
            }
            const value_type operator*(void) const{
                return p[index];
            }
            
            bool operator==(const iterator& rhs) const{
                if(index==rhs.index) return true;  //how to tell if p==rhs.p is a problem
                else return false;
            }
            bool operator!=(const iterator& rhs) const{
                if(index==rhs.index) return false;
                else return true;
            }
            
        };
        
        iterator begin(){
            return iterator(0,*this);
        }
        iterator end(){
            return iterator(s,*this);
        }
    };
    /******* end of proxy for unary operation  ***/
/***************************************************************************************************************************/

/***************************************************************************************************************************/
/******* proxy for binary operation  ***/
    template<typename left, typename right, typename fun>
    class proxy{
    public:
        fun f;
        typename  to_ref<left>::type l;
        typename  to_ref<right>::type r;
        uint64_t s;
        
        
        proxy(const proxy& rhs):l(rhs.l),r(rhs.r),f(rhs.f),s(rhs.s){
        }
        proxy(const left& ll, const right& rr, fun& ff):l(ll),r(rr),f(ff){
            s=ll.size()<rr.size()?ll.size():rr.size();
        }
        using value_type=typename choose_type<typename left::value_type, typename right::value_type>::type;
        
        value_type operator[](uint64_t i){
            if(i>=s){
                throw std::out_of_range{"Index out of boundary!"};
            }else{
                return f.eval(l[i],r[i]);
            }
        }
        
        const value_type operator[](uint64_t i) const{
            if(i>=s){
                throw std::out_of_range{"Index out of boundary!"};
            }else{
                return f.eval(l[i],r[i]);
            }
        }
        
        
        uint64_t size() const{
            return s;
        }
        class iterator{
            uint64_t index;
            proxy p;
        public:
            iterator(uint64_t i, proxy pro):p(pro){
                index=i;
            }
            void operator++(void){
                ++index;
            }
            const value_type operator*(void) const{
                return p[index];
            }
            
            bool operator==(const iterator& rhs) const{
                if(index==rhs.index) return true;
                else return false;
            }
            bool operator!=(const iterator& rhs) const{
                if(index==rhs.index) return false;
                else return true;
            }
        
        };
        
        iterator begin(){
            return iterator(0,*this);
        }
        iterator end(){
            return iterator(s,*this);
        }
    };
    /******* end of proxy for binary operation  ***/
/***************************************************************************************************************************/

    
    template<typename T> using same=T;
    
    template<typename T>
    struct get_ref{
        using type=T&;
    };
    
    template<typename T>
    struct get_ref<vector<T>>{
        using type=T&;
    };
    
    template<typename T>
    struct get_const_ref{
        using type= const T&;
    };
    
    template<typename T>
    struct get_const_ref<vector<T>>{
        using type=const T;
    };
    
    
    
    template<typename left, typename right, typename fun>
    struct get_const_ref<proxy<left,right,fun>>{
        using pre_type=typename choose_type<typename left::value_type, typename right::value_type>::type;
        using type=const pre_type;
    };
    
    template<typename left, typename fun>
    struct get_const_ref<proxy_1<left,fun>>{
        using pre_type=typename fun::result_type;
        using type=const pre_type;
    };
    
    template<typename left, typename right, typename fun>
    struct get_ref<proxy<left,right,fun>>{
        using pre_type=typename choose_type<typename left::value_type, typename right::value_type>::type;
        using type=pre_type;
    };
    
    template<typename left, typename fun>
    struct get_ref<proxy_1<left,fun>>{
        using pre_type=typename fun::result_type;
        using type=pre_type;
    };

/***************************************************************************************************************************/
/****function objects for constructing proxies***/
    template<typename T>
    struct sqrt{
        using result_type=typename std::conditional<iscomplex<T>::value, std::complex<double>, double>::type;
        
        const result_type operator()(const T t) const{
            return (result_type) sqrtl(t);
        }
        
        result_type operator()(const T t){
            return (result_type) sqrtl(t);
        }
    };
    
    

    template<typename T1, typename T2>
    struct add{
        const typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) const{
            return (typename choose_type<T1, T2>::type)l+(typename choose_type<T1, T2>::type)r;
        }
        typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) {
            return (typename choose_type<T1, T2>::type)l+(typename choose_type<T1, T2>::type)r;
        }
    };
    
    
    
    
    template<typename T1, typename T2>
    struct minus{
        const typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) const{
            return (typename choose_type<T1, T2>::type)l-(typename choose_type<T1, T2>::type)r;
        }
        typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) {
            return (typename choose_type<T1, T2>::type)l-(typename choose_type<T1, T2>::type)r;
        }
    };
    
    template<typename T1, typename T2>
    struct mul{
        const typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) const{
            return (typename choose_type<T1, T2>::type)l*(typename choose_type<T1, T2>::type)r;
        }
        typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) {
            return (typename choose_type<T1, T2>::type)l*(typename choose_type<T1, T2>::type)r;
        }
    };
    
    template<typename T1, typename T2>
    struct div{
        typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) {
            return (typename choose_type<T1, T2>::type)l/(typename choose_type<T1, T2>::type)r;
        }
        const typename choose_type<T1, T2>::type eval(const T1 l, const T2 r) const{
            return (typename choose_type<T1, T2>::type)l/(typename choose_type<T1, T2>::type)r;
        }
    };
    
    
    template<typename T>
    struct neg{
        T operator()(const T t) {
            return -t;
        }
        using result_type=T;
        const T operator()(const T t) const{
            return -t;
        }
    };
    /****end of function objects for constructing proxies***/
/***************************************************************************************************************************/
    
/***************************************************************************************************************************/
/*valarray wrapper class*/
    template<typename B>
    class val:  public B{
        /* B is vector concept*/
    public:
        using same<B>::same;
        val(void){}
        friend std::ostream& operator<<(std::ostream& stream, const val t){
            for(int i=0;i<t.size();++i){
                stream<<t[i];
            }
            return stream;
        };
        
        val(const val& t):B(t){
    //        std::cout<<"copy constructor called"<<std::endl;
        }
        
        typename B::value_type sum(void){
            typename B::value_type ans=0;
            for(uint64_t i=0;i<this->size();++i){
                ans+=this->operator[](i);
            }
            return ans;
        }
        
        
        
        val<proxy_1<B, epl::sqrt<typename B::value_type>>> sqrt(void){
            epl::sqrt<typename B::value_type> sf;
            val<proxy_1<B, epl::sqrt<typename B::value_type>>> ans(*this, sf);
            return ans;
        }
        
        template<typename fun>
        val<proxy_1<B, fun>> apply(fun f){
            val<proxy_1<B, fun>> ans(*this, f);
            return ans;
        }
        
        template<typename fun>
        typename fun::result_type accumulate(fun f){
            if(this->size()<1) throw std::out_of_range("This valarray has no element!");
            else if(this->size()==1) return (typename fun::result_type) this->operator[](0);
            else{
                typename fun::result_type ans=f(this->operator[](0), this->operator[](1));
                for(uint64_t i=2;i<this->size();++i){
                    ans=f(this->operator[](i),ans);
                }
                return ans;
            }
        }
        
        template<typename T>
        val& operator=(const T t){
            for(uint64_t i=0;i<this->size();++i)
                this->operator[](i)=(typename B::value_type) t;
            return *this;
        }
        
        template<typename T>
        val& operator=(const val<T>& t){
        //    std::cout<<"op= for val is called"<<std::endl;
            uint64_t s=this->size()<t.size()?this->size():t.size();
            for(uint64_t i=0;i<s;++i)
                this->operator[](i)=t[i];
            return *this;
        }
        
        
        
        template<typename T>
        val& operator=( val<T>& t){
            uint64_t s=this->size()<t.size()?this->size():t.size();
            
            for(uint64_t i=0;i<s;++i)
                    this->operator[](i)=t[i];
            return *this;
        }

        
 /***************************************************************************************************************************/
        /* member function operaotrs*/
        template<typename G>
        val<proxy<B,typename prepare<G>::type,add<typename B::value_type,typename unwrap<G>::type>>> operator+(const G& g){
            add<typename B::value_type,typename unwrap<G>::type>  f;
            return val<proxy<B,typename prepare<G>::type,add<typename B::value_type,typename unwrap<G>::type >>>{*this,g,f};
        }
        
        template<typename G>
        val<proxy<B,typename prepare<G>::type,minus<typename B::value_type,typename unwrap<G>::type>>> operator-(const G& g){
            minus<typename B::value_type,typename unwrap<G>::type>  f;
            return val<proxy<B,typename prepare<G>::type,minus<typename B::value_type,typename unwrap<G>::type >>>{*this,g,f};
        }
        
        template<typename G>
        val<proxy<B,typename prepare<G>::type,mul<typename B::value_type,typename unwrap<G>::type>>> operator*(const G& g){
            mul<typename B::value_type,typename unwrap<G>::type>  f;
            return val<proxy<B,typename prepare<G>::type,mul<typename B::value_type,typename unwrap<G>::type >>>{*this,g,f};
        }
        
        template<typename G>
        val<proxy<B,typename prepare<G>::type,div<typename B::value_type,typename unwrap<G>::type>>> operator/(const G& g){
            div<typename B::value_type,typename unwrap<G>::type>  f;
            return val<proxy<B,typename prepare<G>::type,div<typename B::value_type,typename unwrap<G>::type >>>{*this,g,f};
        }
        
        val<proxy_1<B,neg<typename B::value_type>>> operator-(void){
            neg<typename B::value_type>  f;
            return val<proxy_1<B,neg<typename B::value_type> >>{*this,f};
        }
        /* end of member function operaotr */
/***************************************************************************************************************************/

        template<typename left, typename right, typename fun>
        val(val<proxy<left, right, fun>> t){
        //    std::cout<<"I am called"<<std::endl;
            for(uint64_t i=0;i<t.size();i++){
           //     std::cout<<t[i]<<std::endl;
                this->push_back(t[i]);
            }
            
        };
        
        template<typename left, typename fun>
        val(val<proxy_1<left, fun>> t){
            for(uint64_t i=0;i<t.size();i++){
                this->push_back(t[i]);
            }
            
        };

        typename  get_ref<B>::type operator[](uint64_t i){
            return B::operator[](i);
        }
        
        typename  get_const_ref<B>::type operator[](uint64_t i) const{
            return B::operator[] (i);
        }
        
        
        
    };
    /* end of valarray wrapper class*/
/***************************************************************************************************************************/

    /* defin valarray only for the five types, using substitution failure */
         template<typename T, typename unused=typename scalar<T>::value_type>
         using  valarray = val<vector<T>>;
/***************************************************************************************************************************/

    
    template<typename T>
    struct is_val{
        static constexpr bool value=false;
    };
    
    template<typename T>
    struct is_val<val<T>>{
        static constexpr bool value=true;
    };
    
    
    template<typename T, typename G>
    struct get_type{
        using val_type = val<proxy<typename prepare<T>::type,typename prepare<G>::type,add<typename unwrap<T>::type,typename unwrap<G>::type>>>;
        using scalar_type = typename choose_type<T, G>::type;
        static constexpr bool val_or_not=is_val<T>::value||is_val<G>::value;
        using type=typename std::conditional<val_or_not, val_type, scalar_type>::type;
        
    };
/***************************************************************************************************************************/
    /* operators to support scalar op valarray */
    /* this set of operator, combined with the member function operators, can work well even the client declare a generic 
     template operator like this:
     
     template <typename T1, typename T2>
     void operator+(T1, T2) {
     cout << "passed\n";
     }
     
     This solution requires two operators per operation(one member function and one non member), still short enough and gives the client most freedom
     */
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,G ,add<typename unwrap<T>::type,typename G::value_type>>> operator+(const T& t, const val<G>& g){
        add<typename unwrap<T>::type,typename G::value_type>  f;
        return val<proxy<typename prepare<T>::type, G ,add<typename unwrap<T>::type,typename G::value_type >>>{t,g,f};
    }
    
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,G ,minus<typename unwrap<T>::type,typename G::value_type>>> operator-(const T& t, const val<G>& g){
        minus<typename unwrap<T>::type,typename G::value_type>  f;
        return val<proxy<typename prepare<T>::type, G ,minus<typename unwrap<T>::type,typename G::value_type >>>{t,g,f};
    }
    
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,G ,mul<typename unwrap<T>::type,typename G::value_type>>> operator*(const T& t, const val<G>& g){
        mul<typename unwrap<T>::type,typename G::value_type>  f;
        return val<proxy<typename prepare<T>::type, G ,mul<typename unwrap<T>::type,typename G::value_type >>>{t,g,f};
    }
    
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,G ,div<typename unwrap<T>::type,typename G::value_type>>> operator/(const T& t, const val<G>& g){
        div<typename unwrap<T>::type,typename G::value_type>  f;
        return val<proxy<typename prepare<T>::type, G ,div<typename unwrap<T>::type,typename G::value_type >>>{t,g,f};
    }
/***************************************************************************************************************************/

/***************************************************************************************************************************/
    /* I can also solve the problem using one operator per operation only using the following code.( instead of two,which is the solution above). However, this code doesn't work when the client declare a generic template operator, thus I think the two operator solution is better.
     /*
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,typename prepare<G>::type,add<typename unwrap<T>::type,typename unwrap<G>::type>>> operator+(const T& t, const G& g){
        add<typename unwrap<T>::type,typename unwrap<G>::type>  f;
        
        return val<proxy<typename prepare<T>::type,typename prepare<G>::type,add<typename unwrap<T>::type,typename unwrap<G>::type >>>{t,g,f};
        //pass the first test without typecast for t,g
    }
    
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,typename prepare<G>::type,minus<typename unwrap<T>::type,typename unwrap<G>::type>>> operator-(const T& t, const G& g){
        minus<typename unwrap<T>::type,typename unwrap<G>::type>  f;
        
        return val<proxy<typename prepare<T>::type,typename prepare<G>::type,minus<typename unwrap<T>::type,typename unwrap<G>::type >>>{t,g,f};
        //pass the first test without typecast for t,g
    }
    
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,typename prepare<G>::type,mul<typename unwrap<T>::type,typename unwrap<G>::type>>> operator*(const T& t, const G& g){
        mul<typename unwrap<T>::type,typename unwrap<G>::type>  f;
        
        return val<proxy<typename prepare<T>::type,typename prepare<G>::type,mul<typename unwrap<T>::type,typename unwrap<G>::type >>>{t,g,f};
        //pass the first test without typecast for t,g
    }
    
    template<typename T, typename G>
    val<proxy<typename prepare<T>::type,typename prepare<G>::type,div<typename unwrap<T>::type,typename unwrap<G>::type>>> operator/(const T& t, const G& g){
        div<typename unwrap<T>::type,typename unwrap<G>::type>  f;
        
        return val<proxy<typename prepare<T>::type,typename prepare<G>::type,div<typename unwrap<T>::type,typename unwrap<G>::type >>>{t,g,f};
        //pass the first test without typecast for t,g
    }
     */
/***************************************************************************************************************************/

    

  }
#endif /* _Valarray_h */

