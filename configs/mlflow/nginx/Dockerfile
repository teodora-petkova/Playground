FROM nginx:1.17.6

RUN apt-get update && apt-get install -y apache2 && apt-get install -y apache2-utils


# Remove default Nginx config
RUN rm /etc/nginx/nginx.conf

# Copy the modified Nginx conf
COPY ./nginx.conf /etc/nginx

# Copy proxy config
COPY mlflow.conf /etc/nginx/sites-enabled/

# Gen HTTP Authen USER-PASSWORD
ARG NGINX_HTTP_AUTH_USER
ARG NGINX_HTTP_AUTH_PASSWORD
RUN htpasswd -cb /etc/nginx/.htpasswd ${NGINX_HTTP_AUTH_USER} ${NGINX_HTTP_AUTH_PASSWORD}