<?php
define( 'ROOT_PATH', dirname( dirname( __FILE__ ) ) . DIRECTORY_SEPARATOR );
/** @noinspection PhpIncludeInspection */
require_once ROOT_PATH .'client/php/xtail.php';
XTail::debug( "this is test" );
