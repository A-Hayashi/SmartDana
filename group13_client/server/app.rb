require 'sinatra'
require 'json'

set :bind, '0.0.0.0'
set :port, 8888
set :app_file, __FILE__
 
class Server < Sinatra::Base

  get '/' do
    "Hello World"
  end
  
  put '/api/TAG', provides: :json do
    hash = JSON.parse(request.body.read)
    puts hash
    "number: #{hash["number"]}, online:#{hash["online"]}"
  end


end
