//
//  clickable.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-03-25.
//

#ifndef clickable_h
#define clickable_h

#include <functional>

template <typename T>
class Clickable {
    
protected:
    std::function<void(T*)> clickCallback = [](T*){};
    std::function<void(T*)> hoverCallback = [](T*){};
    std::function<void(T*)> offHoverCallback = [](T*){};
    
public:
    
    virtual void onClick() {
        
    }
    
    virtual void onMouseUp() {
        
    }
    
    virtual void onHover() {
        
    }
    
    virtual void offHover() {
        
    }
    
    virtual void setOnClick(std::function<void(T*)> callback) {
        clickCallback = callback;
    }
    
    virtual void setOnHover(std::function<void(T*)> cb) {
        hoverCallback = cb;
    }
    
    virtual void setOffHover(std::function<void(T*)> cb) {
        offHoverCallback = cb;
    }
    
};


#endif /* clickable_h */
