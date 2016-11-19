#include "Utl_LogMsg.h"


#include "Input.h"

CInput gInputs;


CInput::CInput() {

}

CInput::~CInput() {

}

void CInput::_initKeyMap() {
#define INPUT_KEY_ITEM( k, wkey, n ) _keyMap[wkey] = KEY_##k;
#include "InputKeys.items"
}

void CInput::_initKeyStatesNames() {
    _stateNames[STATE_PRESSED]  = "Pressed";
    _stateNames[STATE_HELD]     = "Held";
    _stateNames[STATE_RELEASED] = "Released";
}

void CInput::_initKeyNames() {
#define INPUT_KEY_ITEM( k, wkey, n ) _keyNames[KEY_##k] = n;
#include "InputKeys.items"
}

CInput::KEY CInput::_getKey( WPARAM _physicalKey ) {
    auto got = _keyMap.find( _physicalKey );
    if( got == _keyMap.end() ) return CInput::KEY::KEY_NONE;
    return got->second;
 }

void CInput::Initialize() {
    _initKeyMap();
    _initKeyNames();
    _initKeyStatesNames();
}

void CInput::Update() {
    // test for one key
    // _printKeyStates( KEY_ESC );
    // _printKeyStates( MK_RIGHT );
    // _printMouseStates();

    // this should be the last step of update,
    // so all stored key states can be used by other module in this frame.
    // 
    // clear pressed and released,
    // since they are all about this frame
    for( int i = KEY_NONE + 1; i < KEY_COUNTER; ++i ) {
        _keyStates[i]._states[STATE_PRESSED] = false;
        _keyStates[i]._states[STATE_RELEASED] = false;
    }
}

// KeyDown is acting like key repeated,
// if we hold the key, 
// KeyDown will be issued multiple times,
// at each repeated interval.
void CInput::OnKeyDown( WPARAM t_key ) {
    // LogMsg << "Key Down" << LogEndl;
    KEY key = _getKey( t_key );
    if( !_isKeyValid( key ) ) return;

    if( GetKeyState( key, STATE_HELD ) ) {
        SetKeyState( key, STATE_PRESSED, false );
    } else {
        SetKeyState( key, STATE_PRESSED, true );
    }

    SetKeyState( key, STATE_HELD, true );
    SetKeyState( key, STATE_RELEASED, false );

}

void CInput::OnKeyUp( WPARAM t_key ) {
    // LogMsg << "Key Up" << LogEndl;
    KEY key = _getKey( t_key );
    if( !_isKeyValid( key ) ) return;

    SetKeyState( key, STATE_PRESSED, false );
    SetKeyState( key, STATE_HELD, false );
    SetKeyState( key, STATE_RELEASED, true );
   
}


// MouseDown is different than KeyDown,
// It's acting as pressed.
// If we hold the mouse button, 
// only the first time will we have MouseDown.
void CInput::OnMouseDown( WPARAM t_btn, WPARAM t_mod, int t_x, int t_y ) {
    // LogMsg << "Mouse Down" << LogEndl;
    KEY key = _getKey( t_btn );
    if( !_isKeyValid( key ) ) return;
    
    SetKeyState( key, STATE_PRESSED, true );
    SetKeyState( key, STATE_HELD, true );
    SetKeyState( key, STATE_RELEASED, false );

    SetMousePosition( t_x, t_y );
}

void CInput::OnMouseUp( WPARAM t_btn, WPARAM t_mod, int t_x, int t_y ) {
    // LogMsg << "Mouse Up" << LogEndl;
    KEY key = _getKey( t_btn );
    if( !_isKeyValid( key ) ) return;

    SetKeyState( key, STATE_PRESSED, false );
    SetKeyState( key, STATE_HELD, false );
    SetKeyState( key, STATE_RELEASED, true );

    SetMousePosition( t_x, t_y );
}

void CInput::OnMouseMove( WPARAM t_btn, int t_x, int t_y ) {
    SetMousePosition( t_x, t_y );
}


bool CInput::GetKeyState( KEY t_key, KEY_STATE t_keyState ) {
    if( t_key >= KEY_COUNTER || t_key <= KEY_NONE ) {
        LogError << "Invalid Key" << LogEndl;
        return false;
    }

    if( t_keyState >= STATE_COUNTER || t_key <= STATE_NONE ) {
        LogError << "Invalid State" << LogEndl;
        return false;
    }

    return _keyStates[t_key]._states[t_keyState];
}

void CInput::SetKeyState( KEY t_key, KEY_STATE t_keyState, bool t_bool ) {
    if( !_isKeyValid( t_key ) ) {
        LogError << "Invalid Key" << LogEndl;
        return;
    }

    if( !_isStateValid( t_keyState ) ) {
        LogError << "Invalid State" << LogEndl;
        return;
    }

    _keyStates[t_key]._states[t_keyState] = t_bool;
}


void CInput::GetMousePosition( int& t_x, int& t_y ) {
    t_x = _mouseStates._x;
    t_y = _mouseStates._y;
}

void CInput::SetMousePosition( int t_x, int t_y ) {
    _mouseStates._x = t_x;
    _mouseStates._y = t_y;
}

void CInput::_printKeyStates( KEY t_key ) {
    if( !_isKeyValid( t_key ) ) return;
    
    const char* keyName = _keyNames[t_key];
    for( int i = STATE_NONE + 1; i < STATE_COUNTER; ++i ) {
        KEY_STATE state = ( KEY_STATE )i;
        if( GetKeyState( t_key, state ) ) {
            LogMsg << keyName << " is " << _stateNames[state] << LogEndl;
        }
    }
}

void CInput::_printMouseStates() {
    LogMsg << "Mouse Position: ( " << _mouseStates._x << ", " << _mouseStates._y << " )" << LogEndl;
}

