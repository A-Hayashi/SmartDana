require 'sinatra'
require 'json'

set :bind, '0.0.0.0'
set :port, 8888
set :app_file, __FILE__
 
class Server < Sinatra::Base
  get '' do
    "elHlo World"
  end
  

  get '/show' do
    "elHlo World"
  end
  
  post '/edit' do
    body = request.body.read
  
    if body == ''
      status 400
    else
      body.to_json
    end
  end

end
