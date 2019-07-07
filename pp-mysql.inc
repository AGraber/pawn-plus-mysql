#if defined _pp_mysql_included
	#endinput
#endif
#define _pp_mysql_included

#if !defined MYSQL_ASYNC_DEFAULT_PARALLEL
#define MYSQL_ASYNC_DEFAULT_PARALLEL false
#endif

native Cache:mysql_query_s(MySQL:handle, AmxString:query, bool:use_cache = true) = mysql_query;
native mysql_tquery_s(MySQL:handle, AmxString:query, const callback[] = "", const format[] = "", {MySQL, Float,_}:...) = mysql_tquery;
native mysql_pquery_s(MySQL:handle, AmxString:query, const callback[] = "", const format[] = "", {MySQL, Float,_}:...) = mysql_pquery;

stock Task:mysql_aquery(MySQL:handle, const query[], parallel = MYSQL_ASYNC_DEFAULT_PARALLEL)
{
	new Task:the_task = task_new();
	new Handle:task_handle = handle_new(the_task, .weak = true);

	new callback_name[3];
	callback_name = amx_encode_value_public_name("__ppAsyncMySQLHandler", task_handle);
	new CallbackHandler:callback_handler = pawn_register_callback("OnQueryError", "__ppAsyncMySQLError", handler_default, "ed", task_handle);
	
	new const ret = parallel ? (mysql_pquery(handle, query, callback_name, "dd", _:callback_handler, _:task_handle)) : (mysql_tquery(handle, query, callback_name, "dd", _:callback_handler, _:task_handle));
	
	if(ret)
	{
		handle_acquire(task_handle);
	}
	else
	{
		pawn_unregister_callback(callback_handler);
		task_set_error_ms(the_task, amx_err_exit, 0);
	}
	return the_task;
}

stock Task:mysql_aquery_s(MySQL:handle, ConstStringTag:query, parallel = MYSQL_ASYNC_DEFAULT_PARALLEL)
{
	new Task:the_task = task_new();
	new Handle:task_handle = handle_new(the_task, .weak = true);

	new callback_name[3];
	callback_name = amx_encode_value_public_name("__ppAsyncMySQLHandler", task_handle);
	new CallbackHandler:callback_handler = pawn_register_callback("OnQueryError", "__ppAsyncMySQLError", handler_default, "ed", task_handle);
	
	new const ret = parallel ? (mysql_pquery_s(handle, str_addr(String:query), callback_name, "dd", _:callback_handler, _:task_handle)) : (mysql_tquery_s(handle, str_addr(String:query), callback_name, "dd", _:callback_handler, _:task_handle));
	
	if(ret)
	{
		handle_acquire(task_handle);
	}
	else
	{
		pawn_unregister_callback(callback_handler);
		task_set_error_ms(the_task, amx_err_exit, 0);
	}
	return the_task;
}

#if !defined OnQueryError
public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle) {}
#endif

forward __ppAsyncMySQLError(CallbackHandler:callback_handler, Handle:task_handle, errorid, const error[], const callback[], const query[], MySQL:handle);
public __ppAsyncMySQLError(CallbackHandler:callback_handler, Handle:task_handle, errorid, const error[], const callback[], const query[], MySQL:handle)
{
	new Handle:encoded_handle;
	if(amx_try_decode_value(callback, encoded_handle))
	{
		if(task_handle == encoded_handle)
		{
			if(handle_linked(task_handle))
			{
				new Task:the_task = Task:handle_get(task_handle);
				task_set_error(the_task, amx_err_exit);
			}
			pawn_unregister_callback(callback_handler);
			handle_release(task_handle);
		}
	}
	return 0;
}

forward __ppAsyncMySQLHandler(CallbackHandler:callback_handler, Handle:task_handle);
public __ppAsyncMySQLHandler(CallbackHandler:callback_handler, Handle:task_handle)
{
	if(handle_linked(task_handle))
	{
		new Task:t = Task:handle_get(task_handle);
		task_set_result(t, true);
	}
	pawn_unregister_callback(callback_handler);
	handle_release(task_handle);
	return 0;
}
