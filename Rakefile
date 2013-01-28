require "bundler/gem_tasks"
require 'fileutils'
require 'rake/testtask'
require 'rake'
require 'rake/extensiontask'

begin
  require 'devkit'
  rescue LoadError
end

Rake::ExtensionTask.new('bson_object_id') do |ext|
  ext.lib_dir = "lib/bson_object_id_ext"
end
