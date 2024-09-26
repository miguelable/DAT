<?php

// Verifica si LOG_ON está definido antes de hacer el log
function log_error($message)
{
    if (defined('LOG_ON') && LOG_ON)
        echo "\033[31m[ERROR] $message\033[0m\n"; // Rojo
}

function log_warning($message)
{
    if (defined('LOG_ON') && LOG_ON)
        echo "\033[33m[WARNING] $message\033[0m\n"; // Amarillo
}

function log_info($message)
{
    if (defined('LOG_ON') && LOG_ON)
        echo "\033[32m[INFO] $message\033[0m\n"; // Verde
}

function log_debug($message)
{
    if (defined('LOG_ON') && LOG_ON)
        echo "\033[34m[DEBUG] $message\033[0m\n"; // Azul
}

function log_verbose($message)
{
    if (defined('LOG_ON') && LOG_ON)
        echo "\033[37m[VERBOSE] $message\033[0m\n"; // Blanco
}