#pragma once

#include <windows.h>
#include <unordered_map>

class CInput {
public:
    CInput();
    ~CInput();

public:
    enum KEY {
        KEY_NONE = 0,

#define INPUT_KEY_ITEM( k, wkey, n ) KEY_##k,
#include "InputKeys.items"

        // counter, don't use it.
        KEY_COUNTER,
        
    };


    enum KEY_STATE {
        STATE_NONE,

        // key is pressed
        STATE_PRESSED,
        // key is being held
        STATE_HELD,
        // key is released
        STATE_RELEASED,

        // counter, don't use it.
        STATE_COUNTER,
    };

private:
    std::unordered_map<WPARAM, KEY> _keyMap;
    std::unordered_map<KEY_STATE, const char*> _stateNames;
    std::unordered_map<KEY, const char*> _keyNames;


    /////////////////////////////////////////////////////////////////
    //
    // Key States
    // 
    // If key is pressed/held/released.
    //
    /////////////////////////////////////////////////////////////////
    struct KEY_STATES { 
        bool _states[STATE_COUNTER];
    };

    KEY_STATES _keyStates[ KEY_COUNTER ];

    /////////////////////////////////////////////////////////////////
    //
    // Mouse states
    // 
    // Mouse related stuff. pointer x/y position
    //
    /////////////////////////////////////////////////////////////////
    struct MOUSE_STATE {
        int _x;
        int _y;
    }_mouseStates;


private:
    void _initKeyMap();
    void _initKeyStatesNames();
    void _initKeyNames();
    
    KEY _getKey( WPARAM _physicalKey );
    void _printKeyStates( KEY t_key );
    void _printMouseStates();

    bool _isKeyValid( KEY t_key ) { return t_key > KEY_NONE && t_key < KEY_COUNTER; }
    bool _isStateValid( KEY_STATE t_keyState ) { return t_keyState > STATE_NONE && t_keyState < STATE_COUNTER; }

public:
    void Initialize();
    void Update();

    void OnKeyDown( WPARAM t_key );
    void OnKeyUp( WPARAM t_key );

    void OnMouseDown( WPARAM t_btn, WPARAM t_mod, int t_x, int t_y );
    void OnMouseUp( WPARAM t_btn, WPARAM t_mod, int t_x, int t_y );
    void OnMouseMove( WPARAM t_btn, int t_x, int t_y );

    bool GetKeyState( KEY t_key, KEY_STATE t_keyState );
    void SetKeyState( KEY t_key, KEY_STATE t_keyState, bool t_bool );

    void GetMousePosition( int& t_x, int& t_y );
    void SetMousePosition( int t_x, int t_y );

};

extern CInput gInputs;