


/**
 *
 * boost::asio::io_service io_service;
 * JVM<basic_thread<thread>> jvm(io_service);
 * jvm.async_exec(method, callback);
 * jvm.async_exec(method2, callback2);
 * io_service.run();
 *
 * Para executar desta forma, preciso colocar o basic_thread sendo o escalonador, e o JVM sendo a JavaVM
 * agora o thread precisa ser uma classe que executa o executionEnginer, ainda nao sei bem ao certo 
 * o que este cara pode ser.
 */
