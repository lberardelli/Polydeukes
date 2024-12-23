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
    std::function<void(std::weak_ptr<T>, glm::vec3)> clickCallback = [](std::weak_ptr<T>, glm::vec3){};
    std::function<void(std::weak_ptr<T>)> hoverCallback = [](std::weak_ptr<T>){};
    std::function<void(std::weak_ptr<T>)> offHoverCallback = [](std::weak_ptr<T>){};
    std::function<void(std::weak_ptr<T>)> onMouseUpCallback = [](std::weak_ptr<T>){};
    std::function<void(std::weak_ptr<T>)> onMouseDragCallback = [](std::weak_ptr<T>){};
    std::function<void(std::weak_ptr<T>)> rightClickCallback = [](std::weak_ptr<T>){};
    std::function<void(std::weak_ptr<T>)> rightClickUpCallback = [](std::weak_ptr<T>){};
    
public:
    
    virtual void onClick(glm::vec3 exactPosition) {
        
    }
    
    virtual void onRightClick() {
        
    }
    
    virtual void onMouseUp() {
        
    }
    
    virtual void onHover() {
        
    }
    
    virtual void offHover() {
        
    }
    
    virtual void onDrag() {
        
    }
    
    virtual void onRightClickUp() {
        
    }
    
    virtual void setOnClick(std::function<void(std::weak_ptr<T>)> callback) {
        clickCallback = std::bind(callback, std::placeholders::_1);
    }
    
    virtual void setOnClick(std::function<void(std::weak_ptr<T>, glm::vec3)> callback) {
        clickCallback = callback;
    }
    
    virtual void setOnRightClick(std::function<void(std::weak_ptr<T>)> cb) {
        rightClickCallback = cb;
    }
    
    virtual void setOnRightClickUp(std::function<void(std::weak_ptr<T>)> cb) {
        rightClickUpCallback = cb;
    }
    
    virtual void setOnHover(std::function<void(std::weak_ptr<T>)> cb) {
        hoverCallback = cb;
    }
    
    virtual void setOffHover(std::function<void(std::weak_ptr<T>)> cb) {
        offHoverCallback = cb;
    }
    
    virtual void setOnMouseUp(std::function<void(std::weak_ptr<T>)> cb) {
        onMouseUpCallback = cb;
    }
    
    virtual  void setOnMouseDrag(std::function<void(std::weak_ptr<T>)> cb) {
        onMouseDragCallback = cb;
    }
    
};


#endif /* clickable_h */
