#include "Valve.h"

#include <Arduino.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wpedantic"
#pragma GCC diagnostic error "-Wreturn-type"

using namespace Knobs;

Valve::Valve( const char * const name, pin_t pin ) : _name( name ), _pin( pin ) {

	_active = false;
	_invert = false;
	_stored = false;
	_inputWhenOff = false;
	_mute = false;
	_locked = false;
}

Valve& Valve::begin() {
	_init();
	return *this;
}

void Valve::_init() {

	if( _inputWhenOff ) {
		_pinMode( _active );
	} else {
		pinMode( _pin, OUTPUT );
	}
}

void Valve::_pinMode( bool to ) {

	if( to ) pinMode( _pin, OUTPUT );
	else pinMode( _pin, INPUT );
}

bool Valve::_modify( bool on ) {

	return _invert ? !on : on;
}

Valve& Valve::invert( bool on ) {
	_invert = on;
	return *this;
}
Valve& Valve::inputWhenOff( bool on ) {
	_inputWhenOff = on;
	return *this;
}
Valve& Valve::enslave( Valve &slave ) {
	_slave = &slave;
	return *this;
}
Valve& Valve::handover( Professor &owner ) {
	_owner = &owner;
	return *this;
}
Valve& Valve::direct( Buttler &listener ) {
	_listener = &listener;
	return *this;
}

void Valve::_turn( bool on ) {

	bool to = _modify( on );

	if( _inputWhenOff ) _pinMode( to );

	digitalWrite( _pin, to );

	if( _slave ) _slave->active( on );
}

Valve& Valve::active( bool on, bool silent ) {

	if( _mute ) return *this;
	if( _locked ) return *this;

	if( _owner ) on = _owner->onChange( *this, _active, on );
	if( _listener && !silent ) _listener->onChange( *this, _active, on );

	_active = on;

	_turn( on );

	return *this;
}

bool Valve::active() {

	return _active;
}



Valve& Valve::store() {

	//_print( "store", _active );

	_stored = _active;
	return *this;
}
Valve& Valve::restore() {

	//_print( "re-store", _stored );

	return active( _stored );
}

Valve& Valve::mute( bool on ) {

	//_print( "M", on );

	_mute = true;
	_turn( on );

	return *this;
}
Valve& Valve::unmute() {

	//_print( "*m*", _active );

	if( !_mute ) return *this;

	_mute = false;
	_turn( _active );

	return *this;
}

bool Valve::muted() {
	return _mute;
}

Valve& Valve::lock() {
	_locked = true;
	return *this;
}
Valve& Valve::unlock() {
	_locked = false;
	return *this;
}
bool Valve::locked() {
	return _locked;
}

pin_t Valve::pin() {

	return _pin;
}
const char * Valve::name() {

	return _name;
}

void Valve::loop( knob_time_t time ) {

	if( _owner ) _owner->onLoop( *this, time );
}

Valve& Valve::_print( const char *msg, bool val ) {

	Serial.print( msg );
	Serial.print( " " );
	Serial.print( _name );
	Serial.print( "(" );
	Serial.print( _pin );
	Serial.print( "/" );
	Serial.print( _invert );
	Serial.print( "> " );
	Serial.println( val );

	return *this;
}

/*
 * T R A N S D U C E R
 */

Transducer::Transducer( const char *name )
		: _name( name ) {}
Transducer::Transducer( const char *name, Valve &v1 )
		: _name( name ) {
	*this << v1;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2 )
		: _name( name ) {
	*this << v1 << v2;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2, Valve &v3 )
		: _name( name ) {
	*this << v1 << v2 << v3;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2, Valve &v3,
		Valve &v4 )
		: _name( name ) {
	*this << v1 << v2 << v3 << v4;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2, Valve &v3,
		Valve &v4, Valve &v5 )
		: _name( name ) {
	*this << v1 << v2 << v3 << v4 << v5;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2, Valve &v3,
		Valve &v4, Valve &v5, Valve &v6 )
		: _name( name ) {
	*this << v1 << v2 << v3 << v4 << v5 << v6;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2, Valve &v3,
		Valve &v4, Valve &v5, Valve &v6, Valve &v7 )
		: _name( name ) {
	*this << v1 << v2 << v3 << v4 << v5 << v6 << v7;
}
Transducer::Transducer( const char *name, Valve &v1, Valve &v2, Valve &v3,
		Valve &v4, Valve &v5, Valve &v6, Valve &v7, Valve &v8 )
		: _name( name ) {
	*this << v1 << v2 << v3 << v4 << v5 << v6 << v7 << v8;
}

Transducer& Transducer::operator <<( Valve &valve ) {

	_valves.add( valve );
	return *this;
}

#define TONALL( m ) \
			Valve *valve; \
			for( valve=_valves.first(); valve; valve=_valves.next() ) { \
					valve->m(); \
			}
#define TONALLP( m, p ) \
			Valve *valve; \
			for( valve=_valves.first(); valve; valve=_valves.next() ) { \
					valve->m( p ); \
			}

Transducer& Transducer::begin() {
	TONALL( begin );
	return *this;
}

inline Transducer& Transducer::active( bool on ) {
	TONALLP( active, on );
	return *this;
}

Transducer& Transducer::on() {
	return active( ON );
}

Transducer& Transducer::off() {
	return active( OFF );
}

Transducer& Transducer::toggle() {
	TONALL( toggle );
	return *this;
}

Transducer& Transducer::store() {
	TONALL( store );
	return *this;
}

Transducer& Transducer::restore() {
	TONALL( restore );
	return *this;
}
Transducer& Transducer::mute( bool on ){
	TONALLP( mute, on );
	return *this;
}
Transducer& Transducer::unmute(){
	TONALL( unmute );
	return *this;
}
Transducer& Transducer::each( transducer_callback_t cb, knob_value_t val ) {

	Valve *valve; \
	for( valve=_valves.first(); valve; valve=_valves.next() ) {

		cb( *this, *valve, val );
	}
	return *this;
}
/*
Transducer& Transducer::active( const char *prefix, knob_value_t val ) {

	unsigned int len = strlen( prefix );

	Valve *valve; \
	for( valve=_valves.first(); valve; valve=_valves.next() ) {

		if( strncmp( name(), prefix, len ) ) valve->active( val );
	}
	return *this;
}
*/
Valve* Transducer::find( const char *prefix ){

	Valve *valve; \
	for( valve=_valves.first(); valve; valve=_valves.next() ) {

		const char *name = valve->name();
		unsigned int len = strlen( name );

		if( strncmp( name, prefix, len ) == 0 ) return valve;
	}
	return NULL;
}

int Transducer::fill() {
	return _valves.fill();
}

Transducer& Transducer::print() {

	Serial.print( "| " );
	Valve *valve; \
	for( valve=_valves.first(); valve; valve=_valves.next() ) {
		Serial.print( valve->active() ? "X" : "o" );
		Serial.print( " " );
	}
	Serial.println( "|" );

	return *this;
}
Transducer& Transducer::activeMask( uint32_t mask ) {

	Valve *valve;
	int count = 1;

	for( valve=_valves.first(); valve; valve=_valves.next() ) {

		valve->active( mask & count );
		count = count<<1;
	}
	return *this;
}

uint32_t Transducer::activeMask() {

	uint32_t mask = 0;
	int count = 1;
	Valve *valve;

	for( valve=_valves.first(); valve; valve=_valves.next() ) {

		mask |= valve->active() ? count : 0;
		count = count<<1;
	}
	return mask;
}

Transducer& Transducer::rotate( t_rotate_f rot ) {

    uint32_t mask = activeMask();

	mask = rot( mask );

    activeMask( mask );
	store();

    return *this;
}
Transducer& Transducer::toggle( t_rotate_f rot ) {

    if( activeMask() ) {

        off();

    } else {
        restore();

        if( !activeMask() ) {
            activeMask( rot( 0 ) );
        }
    }
    return *this;
}

const char * Transducer::name() {
	return _name;
}

void Transducer::loop() {

	Valve *valve;
	knob_time_t now = millis();

	for( valve = _valves.first(); valve; valve = _valves.next() ) {

		valve->loop( now );
	}
}


/*
 * T I M E D  P R O F E S S O R
 */

TimedProfessor::TimedProfessor( knob_time_t holdTime )
		: _holdTime( holdTime )
		, _firstWarning( _TP_FIRST_WARNING )
		, _secondWarning( _TP_SECOND_WARNING )
		{

	_running = false;
}

TimedProfessor::TimedProfessor( knob_time_t holdTime,
		knob_time_t firstWarning, knob_time_t secondWarning )
		: _holdTime( holdTime )
		, _firstWarning( firstWarning )
		, _secondWarning( secondWarning )
		{

	_running = false;
}

void TimedProfessor::start() {

	//Serial.println( "*T/start*" );

	_startTime = millis();
	_running = true;
}

void TimedProfessor::stop() {

	//Serial.println( "*T/stop*" );

	_running = false;
}

void TimedProfessor::reset() {

	if( _running ) start();
}

#define _TIME( val ) ( time > end - (val) )

void TimedProfessor::onLoop( Valve &owner, knob_time_t time ) {

	if( !_running ) return;

	register knob_time_t end = _startTime + _holdTime;

	if( _TIME( 0 ) ){
		owner.active( false ); // stops becuase of callback
	} else if( _secondWarning && _TIME( _secondWarning - _TP_WARNING ) ){
		if( owner.muted() ) {
			owner.unmute();
		}
	} else if( _secondWarning && _TIME( _secondWarning ) ) {
		if( !owner.muted() ){
			owner.mute( false );
		}
	} else if( _firstWarning && _TIME( _firstWarning - _TP_WARNING ) ){
		if( owner.muted() ){
			owner.unmute();
		}
	} else if( _firstWarning && _TIME( _firstWarning ) ) {
		if( !owner.muted() ){
			owner.mute( false );
		}
	}
}

bool TimedProfessor::onChange( Valve &owner, knob_value_t oldVal, knob_value_t newVal ) {

	/*
	Serial.print( "*T/onChange: " );
	Serial.print( oldVal );
	Serial.print( "->" );
	Serial.print( newVal );
	Serial.println( "*" );
	*/

	if( ! oldVal && newVal ) start();
	if( oldVal && !newVal ) stop();

	return newVal;
}

TimedValve::TimedValve( const char * const name, pin_t pin,
		knob_time_t holdTime )
		: Valve( name, pin )
		, _timer( holdTime )
		{

	handover( _timer );
}
TimedValve::TimedValve( const char * const name, pin_t pin,
		knob_time_t holdTime, knob_time_t firstWarning, knob_time_t secondWarning )
		: Valve( name, pin )
		, _timer( holdTime, firstWarning, secondWarning )
		{

	handover( _timer );
}

void TimedValve::keep() {

	_timer.stop();

	mute( false );
	delay( _TP_WARNING );
	unmute();
}

TimedProfessor& TimedValve::timer() {
	return _timer;
}

#pragma GCC diagnostic pop
