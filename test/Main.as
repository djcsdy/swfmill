class Main {
	static function main() : Void {
		debug("set: "+(_root.star.text == "Hello World!" ? "OK" : "FAILED") );
	}

	static function debug( text:String ) {
		_root.scriptout.text += text+"\n";
	}
}
