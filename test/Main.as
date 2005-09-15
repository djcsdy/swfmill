class Main {
	static function main() : Void {
		debug("set: "+(_root.star.text == "Hello World!" ? "OK" : "FAILED") );
		debug("place assignment: "+(_root.star.text2 == "Hello World!" ? "OK" : "FAILED") );
		debug("depth: "+(_root.star.getDepth()+16384)+"==19?" );
	}

	static function debug( text:String ) {
		_root.scriptout.text += text+"\n";
	}
}
