# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'bson_object_id_ext/version'

Gem::Specification.new do |gem|
  gem.name          = "bson_object_id_ext"
  gem.version       = BsonObjectIdExt::VERSION
  gem.authors       = ["getaroom"]
  gem.email         = ["jeem@getaroom.com"]
  gem.description   = %q{C extensions to optimize BSON ObjectIds}
  gem.summary       = %q{C extensions to optimize BSON ObjectIds}
  gem.homepage      = ""

  gem.files         = ['Rakefile'] + Dir['ext/**/*.rb'] + Dir['ext/**/*.c'] + Dir['ext/**/*.h']
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.require_paths = ["ext"]

  gem.add_dependency('mongo')
  gem.add_dependency('rake-compiler')
  gem.has_rdoc = false

  gem.require_paths = ['ext']
  gem.extensions    = ['ext/bson_object_id/extconf.rb']
end
