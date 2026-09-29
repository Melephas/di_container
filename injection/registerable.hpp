#pragma once


namespace injection {
    class container;

    template <class T>
    concept registerable = requires(T a, container& c) {
        T::reg(c);
    };
}
